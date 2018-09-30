//HEADER_GOES_HERE
#ifndef __PLRMSG_H__
#define __PLRMSG_H__

extern int plrmsg_ticks; // weak
extern char plr_msg_slot; // weak
extern _plrmsg plr_msgs[8];

void __fastcall plrmsg_delay(int a1);
char *__fastcall ErrorPlrMsg(char *pszMsg);
size_t EventPlrMsg(char *pszFmt, ...);
void __fastcall SendPlrMsg(int pnum, const char *pszStr);
void __cdecl ClearPlrMsg();
void __cdecl InitPlrMsg();
void __cdecl DrawPlrMsg();
void __fastcall PrintPlrMsg(int no, int x, int y, char *str, int just);

/* rdata */

extern const char text_color_from_player_num[5];

#endif /* __PLRMSG_H__ */
