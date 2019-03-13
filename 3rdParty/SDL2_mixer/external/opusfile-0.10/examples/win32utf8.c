#if defined(_WIN32)
# include <stdio.h>
# include <stdlib.h>
# include <wchar.h>
/*We need the following two to set stdin/stdout to binary.*/
# include <io.h>
# include <fcntl.h>
# define WIN32_LEAN_AND_MEAN
# define WIN32_EXTRA_LEAN
# include <windows.h>
# include "win32utf8.h"

static char *utf16_to_utf8(const wchar_t *_src){
  char   *dst;
  size_t  len;
  size_t  si;
  size_t  di;
  len=wcslen(_src);
  dst=(char *)malloc(sizeof(*dst)*(3*len+1));
  if(dst==NULL)return dst;
  for(di=si=0;si<len;si++){
    unsigned c0;
    c0=_src[si];
    if(c0<0x80){
      /*Can be represented by a 1-byte sequence.*/
      dst[di++]=(char)c0;
      continue;
    }
    else if(c0<0x800){
      /*Can be represented by a 2-byte sequence.*/
      dst[di++]=(char)(0xC0|c0>>6);
      dst[di++]=(char)(0x80|c0&0x3F);
      continue;
    }
    else if(c0>=0xD800&&c0<0xDC00){
      unsigned c1;
      /*This is safe, because c0 was not 0 and _src is NUL-terminated.*/
      c1=_src[si+1];
      if(c1>=0xDC00&&c1<0xE000){
        unsigned w;
        /*Surrogate pair.*/
        w=((c0&0x3FF)<<10|c1&0x3FF)+0x10000;
        /*Can be represented by a 4-byte sequence.*/
        dst[di++]=(char)(0xF0|w>>18);
        dst[di++]=(char)(0x80|w>>12&0x3F);
        dst[di++]=(char)(0x80|w>>6&0x3F);
        dst[di++]=(char)(0x80|w&0x3F);
        si++;
        continue;
      }
    }
    /*Anything else is either a valid 3-byte sequence, an invalid surrogate
       pair, or 'not a character'.
      In the latter two cases, we just encode the value as a 3-byte
       sequence anyway (producing technically invalid UTF-8).
      Later error handling will detect the problem, with a better
       chance of giving a useful error message.*/
    dst[di++]=(char)(0xE0|c0>>12);
    dst[di++]=(char)(0x80|c0>>6&0x3F);
    dst[di++]=(char)(0x80|c0&0x3F);
  }
  dst[di++]='\0';
  return dst;
}

typedef LPWSTR *(APIENTRY *command_line_to_argv_w_func)(LPCWSTR cmd_line,
 int *num_args);

/*Make a best-effort attempt to support UTF-8 on Windows.*/
void win32_utf8_setup(int *_argc,const char ***_argv){
  HMODULE hlib;
  /*We need to set stdin/stdout to binary mode.
    This is unrelated to UTF-8 support, but it's platform specific and we need
     to do it in the same places.*/
  _setmode(_fileno(stdin),_O_BINARY);
  _setmode(_fileno(stdout),_O_BINARY);
  hlib=LoadLibraryA("shell32.dll");
  if(hlib!=NULL){
    command_line_to_argv_w_func command_line_to_argv_w;
    /*This function is only available on Windows 2000 or later.*/
    command_line_to_argv_w=(command_line_to_argv_w_func)GetProcAddress(hlib,
     "CommandLineToArgvW");
    if(command_line_to_argv_w!=NULL){
      wchar_t **argvw;
      int       argc;
      argvw=(*command_line_to_argv_w)(GetCommandLineW(),&argc);
      if(argvw!=NULL){
        int ai;
        /*Really, I don't see why argc would ever differ from *_argc, but let's
           be paranoid.*/
        if(argc>*_argc)argc=*_argc;
        for(ai=0;ai<argc;ai++){
          char *argv;
          argv=utf16_to_utf8(argvw[ai]);
          if(argv!=NULL)(*_argv)[ai]=argv;
        }
        *_argc=argc;
        LocalFree(argvw);
      }
    }
    FreeLibrary(hlib);
  }
# if defined(CP_UTF8)
  /*This does not work correctly in all environments (it breaks output in
     mingw32 for me), and requires a Unicode font (e.g., when using the default
     Raster font, even characters that are available in the font's codepage
     won't display properly).*/
  /*SetConsoleOutputCP(CP_UTF8);*/
# endif
}
#endif
