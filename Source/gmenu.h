//HEADER_GOES_HERE
#ifndef __GMENU_H__
#define __GMENU_H__

extern void *optbar_cel;
extern bool byte_634464; // weak
extern void *PentSpin_cel;
extern TMenuItem *sgpCurrItem;
extern void *BigTGold_cel;
extern int dword_634474; // weak
extern char byte_634478; // weak
extern void (__cdecl *dword_63447C)();
extern TMenuItem *dword_634480; // idb
extern void *option_cel;
extern void *sgpLogo;
extern int dword_63448C; // weak

void __cdecl gmenu_draw_pause();
void __fastcall gmenu_print_text(int x, int y, char *pszStr);
void __cdecl FreeGMenu();
void __cdecl gmenu_init_menu();
bool __cdecl gmenu_exception();
void __fastcall gmenu_call_proc(TMenuItem *pItem, void (__cdecl *gmFunc)());
void __fastcall gmenu_up_down(int a1);
void __cdecl gmenu_draw();
void __fastcall gmenu_draw_menu_item(TMenuItem *pItem, int a2);
void __fastcall gmenu_clear_buffer(int x, int y, int width, int height);
int __fastcall gmenu_get_lfont(TMenuItem *pItem);
int __fastcall gmenu_presskeys(int a1);
void __fastcall gmenu_left_right(int a1);
int __fastcall gmenu_on_mouse_move(LPARAM lParam);
bool __fastcall gmenu_valid_mouse_pos(int *plOffset);
int __fastcall gmenu_left_mouse(int a1);
void __fastcall gmenu_enable(TMenuItem *pMenuItem, bool enable);
void __fastcall gmenu_slider_1(TMenuItem *pItem, int min, int max, int gamma);
int __fastcall gmenu_slider_get(TMenuItem *pItem, int min, int max);
void __fastcall gmenu_slider_3(TMenuItem *pItem, int dwTicks);

/* rdata */

extern const unsigned char lfontframe[127];
extern const unsigned char lfontkern[56];

#endif /* __GMENU_H__ */
