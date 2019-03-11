#if !defined(_win32utf8_H)
# define _win32utf8_H (1)
# if defined(_WIN32)

/*Make a best-effort attempt to support UTF-8 on Windows.*/
void win32_utf8_setup(int *_argc,const char ***_argv);

# endif
#endif
