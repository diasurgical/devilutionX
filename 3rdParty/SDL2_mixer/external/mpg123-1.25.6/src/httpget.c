/*
	httpget.c: http communication

	copyright ?-2011 by the mpg123 project - free software under the terms of the LGPL 2.1
	see COPYING and AUTHORS files in distribution or http://mpg123.org
	initially written Oliver Fromme
	old timestamp: Wed Apr  9 20:57:47 MET DST 1997

	Thomas' notes:
	;
	I used to do 
	GET http://server/path HTTP/1.0

	But RFC 1945 says: The absoluteURI form is only allowed when the request is being made to a proxy.

	so I should not do that. Since name based virtual hosts need the hostname in the request, I still need to provide that info.
	Enter HTTP/1.1... there is a Host eader field to use (that mpg123 supposedly has used since some time anyway - but did it really work with my vhost test server)?
	Now
	GET /path/bla HTTP/1.1\r\nHost: host[:port]
	Should work, but as a funny sidenote:
	
	RFC2616: To allow for transition to absoluteURIs in all requests in future versions of HTTP, all HTTP/1.1 servers MUST accept the absoluteURI form in requests, even though HTTP/1.1 clients will only generate them in requests to proxies.
	
	I was already full-on HTTP/1.1 as I recognized that mpg123 then would have to accept the chunked transfer encoding.
	That is not desireable for its purpose... maybe when interleaving of shoutcasts with metadata chunks is supported, we can upgrade to 1.1.
	Funny aspect there is that shoutcast servers do not do HTTP/1.1 chunked transfer but implement some different chunking themselves...
*/

#include "mpg123app.h"
#include "httpget.h"

#ifdef NETWORK
#include "resolver.h"

#include <errno.h>
#include "true.h"
#endif

#include <ctype.h>

#include "debug.h"

void httpdata_init(struct httpdata *e)
{
	mpg123_init_string(&e->content_type);
	mpg123_init_string(&e->icy_url);
	mpg123_init_string(&e->icy_name);
	e->icy_interval = 0;
	e->proxystate = PROXY_UNKNOWN;
	mpg123_init_string(&e->proxyhost);
	mpg123_init_string(&e->proxyport);
}

void httpdata_reset(struct httpdata *e)
{
	mpg123_free_string(&e->content_type);
	mpg123_free_string(&e->icy_url);
	mpg123_free_string(&e->icy_name);
	e->icy_interval = 0;
	/* the other stuff shall persist */
}

void httpdata_free(struct httpdata *e)
{
	httpdata_reset(e);
	mpg123_free_string(&e->proxyhost);
	mpg123_free_string(&e->proxyport);
}

/* mime type classes */
#define M_FILE 0
#define M_M3U  1
#define M_PLS  2
static const char* mime_file[] =
{
	"audio/mpeg",  "audio/x-mpeg",
	"audio/mp3",   "audio/x-mp3", 
	"audio/mpeg3", "audio/x-mpeg3",
	"audio/mpg",   "audio/x-mpg",
	"audio/x-mpegaudio",
	"application/octet-stream", /* Assume raw binary data is some MPEG data. */
	NULL
};
static const char* mime_m3u[] = { "audio/mpegurl", "audio/mpeg-url", "audio/x-mpegurl", NULL };
static const char* mime_pls[]	=
{
	"audio/x-scpls"
,	"audio/scpls"
,	"application/pls"
,	"application/x-scpls"
,	"application/pls+xml"
,	NULL
};
static const char** mimes[] = { mime_file, mime_m3u, mime_pls, NULL };

int debunk_mime(const char* mime)
{
	int i,j;
	size_t len;
	int r = 0;
	char *aux;
	/* Watch out for such: "audio/x-mpegurl; charset=utf-8" */
	aux = strchr(mime, ';');
	if(aux != NULL)
	{
		if(!param.quiet)
			fprintf(stderr, "Warning: additional info in content-type ignored (%s)\n", aux+1);
		/* Just compare up to before the ";" */
		len = aux-mime;
	}
	/* Else, compare the whole string -- including the end. */
	else len = strlen(mime)+1;

	/* Skip trailing whitespace, to ne nice to strange folks. */
	while(len && isspace(mime[len-1])) --len;

	for(i=0; mimes[i]    != NULL; ++i)
	for(j=0; mimes[i][j] != NULL; ++j)
	if(!strncasecmp(mimes[i][j], mime, len)) goto debunk_result;

debunk_result:
	if(mimes[i] != NULL)
	{
		switch(i)
		{
			case M_FILE: r = IS_FILE;        break;
			case M_M3U:  r = IS_LIST|IS_M3U; break;
			case M_PLS:  r = IS_LIST|IS_PLS; break;
			default: error("unexpected MIME debunk result -- coding error?!");
		}
	}
	return r;
}


#ifdef NETWORK
#if !defined (WANT_WIN32_SOCKETS)
static int writestring (int fd, mpg123_string *string)
{
	size_t result, bytes;
	char *ptr = string->p;
	bytes = string->fill ? string->fill-1 : 0;

	while(bytes)
	{
		result = write(fd, ptr, bytes);
		if(result < 0 && errno != EINTR)
		{
			perror ("writing http string");
			return FALSE;
		}
		else if(result == 0)
		{
			error("write: socket closed unexpectedly");
			return FALSE;
		}
		ptr   += result;
		bytes -= result;
	}
	return TRUE;
}

static size_t readstring (mpg123_string *string, size_t maxlen, int fd)
{
	int err;
	debug2("Attempting readstring on %d for %"SIZE_P" bytes", fd, (size_p)maxlen);
	string->fill = 0;
	while(maxlen == 0 || string->fill < maxlen)
	{
		if(string->size-string->fill < 1)
		if(!mpg123_grow_string(string, string->fill+4096))
		{
			error("Cannot allocate memory for reading.");
			string->fill = 0;
			return 0;
		}
		err = read(fd,string->p+string->fill,1);
		/* Whoa... reading one byte at a time... one could ensure the line break in another way, but more work. */
		if( err == 1)
		{
			string->fill++;
			if(string->p[string->fill-1] == '\n') break;
		}
		else if(errno != EINTR)
		{
			error("Error reading from socket or unexpected EOF.");
			string->fill = 0;
			/* bail out to prevent endless loop */
			return 0;
		}
	}

	if(!mpg123_grow_string(string, string->fill+1))
	{
		string->fill=0;
	}
	else
	{
		string->p[string->fill] = 0;
		string->fill++;
	}
	return string->fill;
}
#endif /* WANT_WIN32_SOCKETS */

void encode64 (char *source,char *destination)
{
  static char *Base64Digits =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int n = 0;
  int ssiz=strlen(source);
  int i;

  for (i = 0 ; i < ssiz ; i += 3) {
    unsigned int buf;
    buf = ((unsigned char *)source)[i] << 16;
    if (i+1 < ssiz)
      buf |= ((unsigned char *)source)[i+1] << 8;
    if (i+2 < ssiz)
      buf |= ((unsigned char *)source)[i+2];

    destination[n++] = Base64Digits[(buf >> 18) % 64];
    destination[n++] = Base64Digits[(buf >> 12) % 64];
    if (i+1 < ssiz)
      destination[n++] = Base64Digits[(buf >> 6) % 64];
    else
      destination[n++] = '=';
    if (i+2 < ssiz)
      destination[n++] = Base64Digits[buf % 64];
    else
      destination[n++] = '=';
  }
  destination[n++] = 0;
}

/* Look out for HTTP header field to parse, construct C string with the value.
   Attention: Modifies argument, since it's so convenient... */
char *get_header_val(const char *hname, mpg123_string *response)
{
	char *tmp = NULL;
	size_t prelen = strlen(hname);
	/* if header name found, next char is at least something, so just check for : */
	if(!strncasecmp(hname, response->p, prelen) && (response->p[prelen] == ':'))
	{
		++prelen;
		if((tmp = strchr(response->p, '\r')) != NULL ) tmp[0] = 0;
		if((tmp = strchr(response->p, '\n')) != NULL ) tmp[0] = 0;
		tmp = response->p+prelen;
		/* I _know_ that there is a terminating zero, so this loop is safe. */
		while((tmp[0] == ' ') || (tmp[0] == '\t'))
		{
			++tmp;
		}
	}
	return tmp;
}

/* Iterate over header field names and storage locations, to possibly get those values. */
void get_header_string(mpg123_string *response, const char *fieldname, mpg123_string *store)
{
	char *tmp;
	if((tmp = get_header_val(fieldname, response)))
	{
		if(mpg123_set_string(store, tmp)){ debug2("got %s %s", fieldname, store->p); return; }
		else{ error2("unable to set %s to %s!", fieldname, tmp); }
	}
}

/* shoutcsast meta data: 1=on, 0=off */

char *httpauth = NULL;

size_t accept_length(void)
{
	int i,j;
	static size_t l = 0;
	if(l) return l;
	l += strlen("Accept: ");
	for(i=0; mimes[i]    != NULL; ++i)
	for(j=0; mimes[i][j] != NULL; ++j){ l += strlen(mimes[i][j]) + strlen(", "); }
	l += strlen("*/*\r\n");
	debug1("initial computation of accept header length: %lu", (unsigned long)l);
	return l;
}

/* Returns TRUE or FALSE for success. */
int proxy_init(struct httpdata *hd)
{
	int ret = TRUE;
	/* If we don't have explicit proxy given, probe the environment. */
	if (!param.proxyurl)
		if (!(param.proxyurl = getenv("MP3_HTTP_PROXY")))
			if (!(param.proxyurl = getenv("http_proxy")))
				param.proxyurl = getenv("HTTP_PROXY");
	/* Now continue if we have something. */
	if (param.proxyurl && param.proxyurl[0] && strcmp(param.proxyurl, "none"))
	{
		mpg123_string proxyurl;
		mpg123_init_string(&proxyurl);
		if(   !mpg123_set_string(&proxyurl, param.proxyurl)
		   || !split_url(&proxyurl, NULL, &hd->proxyhost, &hd->proxyport, NULL))
		{
			error("splitting proxy URL");
			ret = FALSE;
		}
		else if(param.verbose > 1) fprintf(stderr, "Note: Using proxy %s\n", hd->proxyhost.p);
#if 0 /* not yet there */
		if(!try_host_lookup(proxyhost))
		{
			error("Unknown proxy host \"%s\".\n", proxyhost.p);
			ret = FALSE;
		}
#endif
		mpg123_free_string(&proxyurl);
		if(ret) hd->proxystate = PROXY_HOST; /* We got hostname and port settled. */
		else hd->proxystate = PROXY_NONE;
	}
	else hd->proxystate = PROXY_NONE;

	return ret;
}

static int append_accept(mpg123_string *s)
{
	size_t i,j;
	if(!mpg123_add_string(s, "Accept: ")) return FALSE;

	/* We prefer what we know. */
	for(i=0; mimes[i]    != NULL; ++i)
	for(j=0; mimes[i][j] != NULL; ++j)
	{
		if(   !mpg123_add_string(s, mimes[i][j])
			 || !mpg123_add_string(s, ", ") )
		return FALSE;
	}
	/* Well... in the end, we accept everything, trying to make sense with reality. */
	if(!mpg123_add_string(s, "*/*\r\n")) return FALSE;

	return TRUE;
}


/*
	Converts spaces to "%20" ... actually, I have to ask myself why.
	What about converting them to "+" instead? Would make things a lot easier.
	Or, on the other hand... what about avoiding HTML encoding at all?
*/
int translate_url(const char *url, mpg123_string *purl)
{
	const char *sptr;
	/* The length of purl is upper bound by 3*strlen(url) + 1 if
	 * everything in it is a space (%20) - or any encoded character */
	if (strlen(url) >= SIZE_MAX/3)
	{
		error("URL too long. Skipping...");
		return FALSE;
	}
	/* Prepare purl in one chunk, to minimize mallocs. */
	if(!mpg123_resize_string(purl, strlen(url) + 31)) return FALSE;
	/*
	 * 2000-10-21:
	 * We would like spaces to be automatically converted to %20's when
	 * fetching via HTTP.
	 * -- Martin Sjögren <md9ms@mdstud.chalmers.se>
	 * Hm, why only spaces? Maybe one should do this http stuff more properly...
	 */
	if ((sptr = strchr(url, ' ')) == NULL)
	mpg123_set_string(purl, url);
	else
	{ /* Note that sptr is set from the if to this else... */
		const char *urlptr = url;
		mpg123_set_string(purl, "");
		do {
			if(! ( mpg123_add_substring(purl, urlptr, 0, sptr-urlptr)
			       && mpg123_add_string(purl, "%20") ) )
			return FALSE;
			urlptr = sptr + 1;
		} while ((sptr = strchr (urlptr, ' ')) != NULL);
		if(!mpg123_add_string(purl, urlptr)) return FALSE;
	}
	/* now see if a terminating / may be needed */
	if(strchr(purl->p+(strncmp("http://", purl->p, 7) ? 0 : 7), '/') == NULL
	    && !mpg123_add_string(purl, "/"))
	return FALSE;

	return TRUE;
}

int fill_request(mpg123_string *request, mpg123_string *host, mpg123_string *port, mpg123_string *httpauth1, int *try_without_port)
{
	char* ttemp;
	int ret = TRUE;
	const char *icy = param.talk_icy ? icy_yes : icy_no;

	/* hm, my test redirection had troubles with line break before HTTP/1.0 */
	if((ttemp = strchr(request->p,'\r')) != NULL){ *ttemp = 0; request->fill = ttemp-request->p+1; }

	if((ttemp = strchr(request->p,'\n')) != NULL){ *ttemp = 0; request->fill = ttemp-request->p+1; }

	/* Fill out the request further... */
	if(   !mpg123_add_string(request, " HTTP/1.0\r\nUser-Agent: ")
		 || !mpg123_add_string(request, PACKAGE_NAME)
		 || !mpg123_add_string(request, "/")
		 || !mpg123_add_string(request, PACKAGE_VERSION)
		 || !mpg123_add_string(request, "\r\n") )
	return FALSE;

	if(host->fill)
	{ /* Give virtual hosting a chance... adding the "Host: ... " line. */
		debug2("Host: %s:%s", host->p, port->p);
		if(    mpg123_add_string(request, "Host: ")
			  && mpg123_add_string(request, host->p)
			  && ( *try_without_port || (
			         mpg123_add_string(request, ":")
			      && mpg123_add_string(request, port->p) ))
			  && mpg123_add_string(request, "\r\n") )
		{
			if(*try_without_port) *try_without_port = 0;
		}
		else return FALSE;
	}

	/* Acceptance, stream setup. */
	if(   !append_accept(request)
		 || !mpg123_add_string(request, CONN_HEAD)
		 || !mpg123_add_string(request, icy) )
	return FALSE;

	/* Authorization. */
	if (httpauth1->fill || httpauth) {
		char *buf;
		if(!mpg123_add_string(request,"Authorization: Basic ")) return FALSE;
		if(httpauth1->fill) {
			if(httpauth1->fill > SIZE_MAX / 4) return FALSE;

			buf=(char *)malloc(httpauth1->fill * 4);
			if(!buf)
			{
				error("malloc() failed for http auth, out of memory.");
				return FALSE;
			}
			encode64(httpauth1->p,buf);
		} else {
			if(strlen(httpauth) > SIZE_MAX / 4 - 4 ) return FALSE;

			buf=(char *)malloc((strlen(httpauth) + 1) * 4);
			if(!buf)
			{
				error("malloc() for http auth failed, out of memory.");
				return FALSE;
			}
			encode64(httpauth,buf);
		}

		if( !mpg123_add_string(request, buf) || !mpg123_add_string(request, "\r\n"))
		ret = FALSE;

		free(buf); /* Watch out for leaking if you introduce returns before this line. */
	}
	if(ret) ret = mpg123_add_string(request, "\r\n");

	return ret;
}
#if !defined (WANT_WIN32_SOCKETS)
static int resolve_redirect(mpg123_string *response, mpg123_string *request_url, mpg123_string *purl)
{
	debug1("request_url:%s", request_url->p);
	/* initialized with full old url */
	if(!mpg123_copy_string(request_url, purl)) return FALSE;

	/* We may strip it down to a prefix ot totally. */
	if(strncasecmp(response->p, "Location: http://", 17))
	{ /* OK, only partial strip, need prefix for relative path. */
		char* ptmp = NULL;
		/* though it's not RFC (?), accept relative URIs as wget does */
		fprintf(stderr, "NOTE: no complete URL in redirect, constructing one\n");
		/* not absolute uri, could still be server-absolute */
		/* I prepend a part of the request... out of the request */
		if(response->p[10] == '/')
		{
			/* only prepend http://server/ */
			/* I null the first / after http:// */
			ptmp = strchr(purl->p+7,'/');
			if(ptmp != NULL){ purl->fill = ptmp-purl->p+1; purl->p[purl->fill-1] = 0; }
		}
		else
		{
			/* prepend http://server/path/ */
			/* now we want the last / */
			ptmp = strrchr(purl->p+7, '/');
			if(ptmp != NULL){ purl->fill = ptmp-purl->p+2; purl->p[purl->fill-1] = 0; }
		}
	}
	else purl->fill = 0;

	debug1("prefix=%s", purl->fill ? purl->p : "");
	if(!mpg123_add_string(purl, response->p+10)) return FALSE;

	debug1("           purl: %s", purl->p);
	debug1("old request_url: %s", request_url->p);

	return TRUE;
}

int http_open(char* url, struct httpdata *hd)
{
	mpg123_string purl, host, port, path;
	mpg123_string request, response, request_url;
	mpg123_string httpauth1;
	int sock = -1;
	int oom  = 0;
	int relocate, numrelocs = 0;
	int got_location = FALSE;
	/*
		workaround for http://www.global24music.com/rautemusik/files/extreme/isdn.pls
		this site's apache gives me a relocation to the same place when I give the port in Host request field
		for the record: Apache/2.0.51 (Fedora)
	*/
	int try_without_port = 0;
	mpg123_init_string(&purl);
	mpg123_init_string(&host);
	mpg123_init_string(&port);
	mpg123_init_string(&path);
	mpg123_init_string(&request);
	mpg123_init_string(&response);
	mpg123_init_string(&request_url);
	mpg123_init_string(&httpauth1);

	/* Get initial info for proxy server. Once. */
	if(hd->proxystate == PROXY_UNKNOWN && !proxy_init(hd)) goto exit;

	if(!translate_url(url, &purl)){ oom=1; goto exit; }

	/* Don't confuse the different auth strings... */
	if(!split_url(&purl, &httpauth1, NULL, NULL, NULL) ){ oom=1; goto exit; }

	/* "GET http://"		11
	 * " HTTP/1.0\r\nUser-Agent: <PACKAGE_NAME>/<PACKAGE_VERSION>\r\n"
	 * 				26 + PACKAGE_NAME + PACKAGE_VERSION
	 * accept header            + accept_length()
	 * "Authorization: Basic \r\n"	23
	 * "\r\n"			 2
	 * ... plus the other predefined header lines
	 */
	/* Just use this estimate as first guess to reduce malloc calls in string library. */
	{
		size_t length_estimate = 62 + strlen(PACKAGE_NAME) + strlen(PACKAGE_VERSION) 
		                       + accept_length() + strlen(CONN_HEAD) + strlen(icy_yes) + purl.fill;
		if(    !mpg123_grow_string(&request, length_estimate)
		    || !mpg123_grow_string(&response,4096) )
		{
			oom=1; goto exit;
		}
	}

	do
	{
		/* Storing the request url, with http:// prepended if needed. */
		/* used to be url here... seemed wrong to me (when loop advanced...) */
		if(strncasecmp(purl.p, "http://", 7) != 0) mpg123_set_string(&request_url, "http://");
		else mpg123_set_string(&request_url, "");

		mpg123_chomp_string(&purl);
		mpg123_add_string(&request_url, purl.p);

		/* Always store the host and port from the URL for correct host header
		   in the request. Proxy server is used for connection, but never in the
		   host header! */
		if(!split_url(&purl, NULL, &host, &port, &path)){ oom=1; goto exit; }
		if (hd->proxystate >= PROXY_HOST)
		{
			/* We will connect to proxy, full URL goes into the request. */
			if(    !mpg123_set_string(&request, "GET ")
			    || !mpg123_add_string(&request, request_url.p) )
			{
				oom=1; goto exit;
			}
		}
		else
		{
			/* We will connect to the host from the URL and only the path goes into the request. */
			if(    !mpg123_set_string(&request, "GET ")
			    || !mpg123_add_string(&request, path.p) )
			{
				oom=1; goto exit;
			}
		}

		if(!fill_request(&request, &host, &port, &httpauth1, &try_without_port)){ oom=1; goto exit; }

		httpauth1.fill = 0; /* We use the auth data from the URL only once. */
		if (hd->proxystate >= PROXY_HOST)
		{
			/* Only the host:port used for actual connection is replaced by
			   proxy. */
			if(    !mpg123_copy_string(&hd->proxyhost, &host)
			    || !mpg123_copy_string(&hd->proxyport, &port) )
			{
				oom=1; goto exit;
			}
		}
		debug2("attempting to open_connection to %s:%s", host.p, port.p);
		sock = open_connection(&host, &port);
		if(sock < 0)
		{
			error1("Unable to establish connection to %s", host.fill ? host.p : "");
			goto exit;
		}
#define http_failure close(sock); sock=-1; goto exit;
		
		if(param.verbose > 2) fprintf(stderr, "HTTP request:\n%s\n",request.p);
		if(!writestring(sock, &request)){ http_failure; }
		relocate = FALSE;
		/* Arbitrary length limit here... */
#define safe_readstring \
		readstring(&response, SIZE_MAX/16, sock); \
		if(response.fill > SIZE_MAX/16) /* > because of appended zero. */ \
		{ \
			error("HTTP response line exceeds max. length"); \
			http_failure; \
		} \
		else if(response.fill == 0) \
		{ \
			error("readstring failed"); \
			http_failure; \
		} \
		if(param.verbose > 2) fprintf(stderr, "HTTP in: %s", response.p);
		safe_readstring;

		{
			char *sptr;
			if((sptr = strchr(response.p, ' ')))
			{
				if(response.fill > sptr-response.p+2)
				switch (sptr[1])
				{
					case '3':
						relocate = TRUE;
					case '2':
						break;
					default:
						fprintf (stderr, "HTTP request failed: %s", sptr+1); /* '\n' is included */
						http_failure;
				}
				else{ error("Too short response,"); http_failure; }
			}
		}

		/* If we are relocated, we need to look out for a Location header. */
		got_location = FALSE;

		do
		{
			safe_readstring; /* Think about that: Should we really error out when we get nothing? Could be that the server forgot the trailing empty line... */
			if (!strncasecmp(response.p, "Location: ", 10))
			{ /* It is a redirection! */
				if(!resolve_redirect(&response, &request_url, &purl)){ oom=1, http_failure; }

				if(!strcmp(purl.p, request_url.p))
				{
					warning("relocated to very same place! trying request again without host port");
					try_without_port = 1;
				}
				got_location = TRUE;
			}
			else
			{ /* We got a header line (or the closing empty line). */
				char *tmp;
				debug1("searching for header values... %s", response.p);
				/* Not sure if I want to bail out on error here. */
				/* Also: What text encoding are these strings in? Doesn't need to be plain ASCII... */
				get_header_string(&response, "content-type", &hd->content_type);
				get_header_string(&response, "icy-name",     &hd->icy_name);
				get_header_string(&response, "icy-url",      &hd->icy_url);

				/* watch out for icy-metaint */
				if((tmp = get_header_val("icy-metaint", &response)))
				{
					hd->icy_interval = (off_t) atol(tmp); /* atoll ? */
					debug1("got icy-metaint %li", (long int)hd->icy_interval);
				}
			}
		} while(response.p[0] != '\r' && response.p[0] != '\n');
		if(relocate)
		{
			close(sock);
			sock = -1;
			/* Forget content type, might just relate to a displayed error page,
			   not the resource being redirected to. */
			mpg123_free_string(&hd->content_type);
			mpg123_init_string(&hd->content_type);
		}
	} while(relocate && got_location && purl.fill && numrelocs++ < HTTP_MAX_RELOCATIONS);
	if(relocate)
	{
		if(!got_location)
		error("Server meant to redirect but failed to provide a location!");
		else
		error1("Too many HTTP relocations (%i).", numrelocs);

		http_failure;
	}

exit: /* The end as well as the exception handling point... */
	if(oom) error("Apparently, I ran out of memory or had some bad input data...");

	mpg123_free_string(&purl);
	mpg123_free_string(&host);
	mpg123_free_string(&port);
	mpg123_free_string(&path);
	mpg123_free_string(&request);
	mpg123_free_string(&response);
	mpg123_free_string(&request_url);
	mpg123_free_string(&httpauth1);
	return sock;
}
#endif /*WANT_WIN32_SOCKETS*/

#else /* NETWORK */

/* stub */
int http_open (char* url, struct httpdata *hd)
{
	if(!param.quiet)
		error("HTTP support not built in.");
	return -1;
}
#endif

/* EOF */

