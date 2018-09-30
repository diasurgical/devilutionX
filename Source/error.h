//HEADER_GOES_HERE
#ifndef __ERROR_H__
#define __ERROR_H__

extern char msgtable[80];
extern char msgdelay; // weak
extern char msgflag; // weak
extern char msgcnt; // weak

void __fastcall InitDiabloMsg(char e);
void __cdecl ClrDiabloMsg();
void __cdecl DrawDiabloMsg();

/* data */
extern char *MsgStrings[44];

#endif /* __ERROR_H__ */
