//HEADER_GOES_HERE
#ifndef __HELP_H__
#define __HELP_H__

extern int help_select_line; // weak
extern int dword_634494; // weak
extern int helpflag;
extern int displayinghelp[22];
extern int HelpTop; // weak

void __cdecl InitHelp();
void __cdecl DrawHelp();
void __fastcall DrawHelpLine(int always_0, int help_line_nr, char *text, text_color color);
void __cdecl DisplayHelp();
void __cdecl HelpScrollUp();
void __cdecl HelpScrollDown();

/* rdata */
extern const char gszHelpText[];

#endif /* __HELP_H__ */
