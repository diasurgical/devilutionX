#include "config.h"
#include "mpg123.h"
#include "mpg123app.h"
#include "httpget.h"
#include "debug.h"
#include "resolver.h"
#include "compat.h"
#include <errno.h>

#if defined (WANT_WIN32_SOCKETS)
#ifdef DEBUG
#define msgme(x) win32_net_msg(x,__FILE__,__LINE__)
#define msgme1 win32_net_msg(1,__FILE__,__LINE__)
#define msgme_sock_err(x) if ((x)==SOCKET_ERROR) {msgme1;}
#else
#define msgme(x) x
#define msgme1 do{} while(0)
#define msgme_sock_err(x) x
#endif
struct ws_local
{
  int inited;
  SOCKET local_socket; /*stores last connet in win32_net_open_connection*/
  WSADATA wsadata;
};

static struct ws_local ws;
#ifdef DEBUG
static void win32_net_msg (const int err, const char * const filedata, const int linedata)
{
  char *errbuff;
  int lc_err;
  if (err)
  {
    lc_err = WSAGetLastError();
    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      lc_err,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &errbuff,
      0,
      NULL );
    fprintf(stderr, "[%s:%d] [WSA2: %d] %s", filedata, linedata, lc_err, errbuff);
    LocalFree (errbuff);
  }
}
#endif

void win32_net_init (void)
{
  ws.inited = 1;
  switch ((WSAStartup(MAKEWORD(2,2), &ws.wsadata)))
  {
    case WSASYSNOTREADY: debug("WSAStartup failed with WSASYSNOTREADY"); break;
    case WSAVERNOTSUPPORTED: debug("WSAStartup failed with WSAVERNOTSUPPORTED"); break;
    case WSAEINPROGRESS: debug("WSAStartup failed with WSAEINPROGRESS"); break;
    case WSAEPROCLIM: debug("WSAStartup failed with WSAEPROCLIM"); break;
    case WSAEFAULT: debug("WSAStartup failed with WSAEFAULT"); break;
    default:
    break;
  }
}

void win32_net_deinit (void)
{
  debug("Begin winsock cleanup");
  if (ws.inited)
  {
    if (ws.inited >= 2 && ws.local_socket != SOCKET_ERROR)
    {
      debug1("ws.local_socket = %"SIZE_P"", (size_p)ws.local_socket);
      msgme_sock_err(shutdown(ws.local_socket, SD_BOTH));
      win32_net_close(ws.local_socket);
    }
    WSACleanup();
    ws.inited = 0;
  }
}

void win32_net_close (int sock)
{
    msgme_sock_err(closesocket(ws.local_socket));
}

static void win32_net_nonblock(int sock)
{
  u_long mode = 1;
  msgme_sock_err(ioctlsocket(ws.local_socket, FIONBIO, &mode));
}

static void win32_net_block(int sock)
{
  u_long mode = 0;
  msgme_sock_err(ioctlsocket(ws.local_socket, FIONBIO, &mode));
}

ssize_t win32_net_read (int fildes, void *buf, size_t nbyte)
{
  debug1("Attempting to read %"SIZE_P" bytes from network.", (size_p)nbyte);
  ssize_t ret;
  msgme_sock_err(ret = (ssize_t) recv(ws.local_socket, buf, nbyte, 0));
  debug1("Read %"SSIZE_P" bytes from network.", (ssize_p)ret);

  return ret;
}

static int get_sock_ch (int sock)
{
  char c;
  int ret;
  msgme_sock_err(ret = recv (ws.local_socket, &c, 1, 0));
  if (ret == 1)
    return (((int) c)&0xff);
  return -1;
}
/* Addapted from from newlib*/
char *win32_net_fgets(char *s, int n, int stream)
{
  char c = 0;
  char *buf;
  buf = s;
  debug1("Pseudo net fgets attempts to read %d bytes from network.", n - 1);
  while (--n > 0 && (c = get_sock_ch (stream)) != -1)
  {
    *s++ = c;
    if (c == '\n' || c == '\r')
      break;
  }
  debug1("Pseudo net fgets got %"SIZE_P" bytes.", (size_p)(s - buf));
  if (c == -1 && s == buf)
  {
    debug("Pseudo net fgets met a premature end.");
    return NULL;
  }
  *s = 0;
  return buf;
}

ssize_t win32_net_write (int fildes, const void *buf, size_t nbyte)
{
  debug1("Attempting to write %"SIZE_P" bytes to network.", (size_p)nbyte);
  ssize_t ret;
  msgme_sock_err((ret = (ssize_t) send(ws.local_socket, buf, nbyte, 0)));
  debug1("wrote %"SSIZE_P" bytes to network.", (ssize_t)ret);

  return ret;
}

off_t win32_net_lseek (int a, off_t b, int c)
{
  debug("lseek on a socket called!");
  return -1;
}

void win32_net_replace (mpg123_handle *fr)
{
  debug("win32_net_replace ran");
  mpg123_replace_reader(fr, win32_net_read, win32_net_lseek);
}

static int win32_net_timeout_connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
{
	debug("win32_net_timeout_connect ran");
	if(param.timeout > 0)
	{
		int err;
		win32_net_nonblock(ws.local_socket);
		err = connect(ws.local_socket, serv_addr, addrlen);
		if(err != SOCKET_ERROR)
		{
			debug("immediately successful");
			win32_net_block(ws.local_socket);
			return 0;
		}
		else if(WSAGetLastError() == WSAEWOULDBLOCK) /*WSAEINPROGRESS would not work here for some reason*/
		{
			struct timeval tv;
			fd_set fds;
			tv.tv_sec = param.timeout;
			tv.tv_usec = 0;

			debug("in progress, waiting...");

			FD_ZERO(&fds);
			FD_SET(ws.local_socket, &fds);
			err = select(ws.local_socket+1, NULL, &fds, NULL, &tv);
			if(err != SOCKET_ERROR)
			{
				socklen_t len = sizeof(err);
				if((getsockopt(ws.local_socket, SOL_SOCKET, SO_ERROR, (char *)&err, &len) != SOCKET_ERROR)
				   && (err == 0) )
				{
					debug("non-blocking connect has been successful");
					win32_net_block(ws.local_socket);
					return 0;
				}
				else
				{
					//error1("connection error: %s", msgme(err));
					return -1;
				}
			}
			else if(err == 0)
			{
				error("connection timed out");
				return -1;
			}
			else
			{
				/*error1("error from select(): %s", strerror(errno));*/
				debug("error from select():");
				msgme1;
				return -1;
			}
		}
		else
		{
			/*error1("connection failed: %s", strerror(errno));*/
			debug("connection failed: ");
			msgme1;
			return err;
		}
	}
	else
	{
		if(connect(ws.local_socket, serv_addr, addrlen) == SOCKET_ERROR)
		{
			/*error1("connection failed: %s", strerror(errno));*/
			debug("connection failed");
			msgme1;
			return -1;
		}
		else {
		debug("win32_net_timeout_connect succeed");
		return 0; /* _good_ */
		}
	}
}

static int win32_net_open_connection(mpg123_string *host, mpg123_string *port)
{
	struct addrinfo hints;
	struct addrinfo *addr, *addrlist;
	SOCKET addrcount;
	ws.local_socket = SOCKET_ERROR;

	if(param.verbose>1) fprintf(stderr, "Note: Attempting new-style connection to %s\n", host->p);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_UNSPEC; /* We accept both IPv4 and IPv6 ... and perhaps IPv8;-) */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	debug2("Atempt resolve/connect to %s:%s", host->p, port->p);
	msgme(addrcount = getaddrinfo(host->p, port->p, &hints, &addrlist));

	if(addrcount == INVALID_SOCKET)
	{
		error3("Resolving %s:%s: %s", host->p, port->p, gai_strerror(addrcount));
		return -1;
	}

	addr = addrlist;
	while(addr != NULL)
	{
		ws.local_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (ws.local_socket == INVALID_SOCKET)
		{
			msgme1;
		}
		else
		{
			if(win32_net_timeout_connect(ws.local_socket, addr->ai_addr, addr->ai_addrlen) == 0)
			break;
			debug("win32_net_timeout_connect error, closing socket");
			win32_net_close(ws.local_socket);
			ws.local_socket=SOCKET_ERROR;
		}
		addr=addr->ai_next;
	}
	if(ws.local_socket == SOCKET_ERROR) {error2("Cannot resolve/connect to %s:%s!", host->p, port->p);}
	else
	{
	  ws.inited = 2;
	}

	freeaddrinfo(addrlist);
	return 1;
}

static size_t win32_net_readstring (mpg123_string *string, size_t maxlen, int fd)
{
	debug2("Attempting readstring on %d for %"SIZE_P" bytes", fd, (size_p)maxlen);
	int err;
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
		err = win32_net_read(0,string->p+string->fill,1); /*fd is ignored */
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

static int win32_net_writestring (int fd, mpg123_string *string)
{
	size_t result, bytes;
	char *ptr = string->p;
	bytes = string->fill ? string->fill-1 : 0;

	while(bytes)
	{
		result = win32_net_write(ws.local_socket, ptr, bytes);
		if(result < 0 && WSAGetLastError() != WSAEINTR)
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

static int win32_net_resolve_redirect(mpg123_string *response, mpg123_string *request_url, mpg123_string *purl)
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

int win32_net_http_open(char* url, struct httpdata *hd)
{
	mpg123_string purl, host, port, path;
	mpg123_string request, response, request_url;
	mpg123_string httpauth1;
	ws.local_socket = SOCKET_ERROR;
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

		mpg123_add_string(&request_url, purl.p);

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
			if(    !mpg123_copy_string(&hd->proxyhost, &host)
			    || !mpg123_copy_string(&hd->proxyport, &port) )
			{
				oom=1; goto exit;
			}
		}
		debug2("attempting to open_connection to %s:%s", host.p, port.p);
		win32_net_open_connection(&host, &port);
		if(ws.local_socket == SOCKET_ERROR)
		{
			error1("Unable to establish connection to %s", host.fill ? host.p : "");
			goto exit;
		}
		debug("win32_net_open_connection succeed");
#define http_failure win32_net_close(ws.local_socket); ws.local_socket=SOCKET_ERROR; goto exit;
		
		if(param.verbose > 2) fprintf(stderr, "HTTP request:\n%s\n",request.p);
		if(!win32_net_writestring(ws.local_socket, &request)){ http_failure; }
		debug("Skipping fdopen for WSA sockets");
		relocate = FALSE;
		/* Arbitrary length limit here... */
#define safe_readstring \
		win32_net_readstring(&response, SIZE_MAX/16, -1); \
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
				if(!win32_net_resolve_redirect(&response, &request_url, &purl)){ oom=1, http_failure; }

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
		if (relocate) { win32_net_close(ws.local_socket); ws.local_socket=SOCKET_ERROR; }
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
	if (ws.local_socket == SOCKET_ERROR || oom)
	return -1;
	else
	return 1;
}
#else
int win32_net_http_open(char* url, struct httpdata *hd)
{
  return -1;
}
#endif /*WANT_WIN32_SOCKETS */
