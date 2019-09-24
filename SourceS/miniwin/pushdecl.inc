#ifndef DEVILUTION_ENGINE
#pragma push_macro("__cdecl")
#pragma push_macro("__fastcall")
#pragma push_macro("__stdcall")
#pragma push_macro("CALLBACK")
#pragma push_macro("APIENTRY")
#pragma push_macro("WINAPI")
#pragma push_macro("WINAPIV")
#endif

#undef __cdecl
#undef __fastcall
#undef __stdcall
#undef CALLBACK
#undef APIENTRY
#undef WINAPI
#undef WINAPIV
#define __cdecl
#define __fastcall
#define __stdcall
#define CALLBACK
#define APIENTRY
#define WINAPI
#define WINAPIV
