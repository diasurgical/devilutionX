dnl Check whether the AI_ADDRCONFIG flag can be used with getaddrinfo
dnl Taken from APR ...
AC_DEFUN([APR_CHECK_GETADDRINFO_ADDRCONFIG], [
  AC_CACHE_CHECK(for working AI_ADDRCONFIG, apr_cv_gai_addrconfig, [
  AC_TRY_RUN([
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

int main(int argc, char **argv) {
    struct addrinfo hints, *ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_ADDRCONFIG;
    return getaddrinfo("localhost", NULL, &hints, &ai) != 0;
}], [apr_cv_gai_addrconfig=yes], 
    [apr_cv_gai_addrconfig=no],
    [apr_cv_gai_addrconfig=no])])

if test $apr_cv_gai_addrconfig = yes; then
   AC_DEFINE(HAVE_GAI_ADDRCONFIG, 1, [Define if getaddrinfo accepts the AI_ADDRCONFIG flag])
fi
])
