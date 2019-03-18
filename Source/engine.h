//HEADER_GOES_HERE
#ifndef __ENGINE_H__
#define __ENGINE_H__

//offset 0
//pCelBuff->pFrameTable[0]

extern char gbPixelCol;  // automap pixel color 8-bit (palette entry)
extern int dword_52B970; // bool flip - if y < x
extern int orgseed;      // weak
extern int SeedCount;    // weak
extern int dword_52B99C; // bool valid - if x/y are in bounds

void __fastcall CelDrawDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall CelDecodeOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void __fastcall CelDecDatOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth);
void __fastcall CelDrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall CelDecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall CelDecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall CelDecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall CelDecodeLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth);
void __fastcall CelDecodeHdrLightOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall CelDecodeHdrLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall CelDrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light);
void __fastcall Cel2DecDatOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall Cel2DrawHdrOnly(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cel2DecodeHdrOnly(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cel2DecDatLightOnly(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall Cel2DecDatLightTrans(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall Cel2DecodeHdrLight(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cel2DecodeLightTrans(BYTE *pBuff, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cel2DrawHdrLightRed(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light);
void __fastcall CelDecodeRect(BYTE *pBuff, int always_0, int hgt, int wdt, BYTE *pCelBuff, int nCel, int nWidth);
void __fastcall CelDecodeClr(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall CelDrawHdrClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall ENG_set_pixel(int screen_x, int screen_y, UCHAR pixel);
void __fastcall engine_draw_pixel(int x, int y);
void __fastcall DrawLine(int x0, int y0, int x1, int y1, UCHAR col);
int __fastcall GetDirection(int x1, int y1, int x2, int y2);
void __fastcall SetRndSeed(int s);
int __cdecl GetRndSeed();
int __fastcall random(BYTE idx, int v);
void __cdecl mem_init_mutex();
void __cdecl mem_atexit_mutex();
void __cdecl mem_free_mutex(void);
unsigned char *__fastcall DiabloAllocPtr(int dwBytes);
void __fastcall mem_free_dbg(void *p);
BYTE *__fastcall LoadFileInMem(char *pszName, int *pdwFileLen);
void __fastcall LoadFileWithMem(char *pszName, void *buf);
void __fastcall Cl2ApplyTrans(BYTE *p, BYTE *ttbl, int nCel);
void __fastcall Cl2DecodeFrm1(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cl2DecDatFrm1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall Cl2DecodeFrm2(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cl2DecDatFrm2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col);
void __fastcall Cl2DecodeFrm3(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light);
void __fastcall Cl2DecDatLightTbl1(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable);
void __fastcall Cl2DecodeLightTbl(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cl2DecodeFrm4(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cl2DecDatFrm4(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth);
void __fastcall Cl2DecodeClrHL(char col, int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall Cl2DecDatClrHL(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, char col);
void __fastcall Cl2DecodeFrm5(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir, char light);
void __fastcall Cl2DecDatLightTbl2(BYTE *pDecodeTo, BYTE *pRLEBytes, int nDataSize, int nWidth, BYTE *pTable);
void __fastcall Cl2DecodeFrm6(int sx, int sy, BYTE *pCelBuff, int nCel, int nWidth, int always_0, int dir);
void __fastcall PlayInGameMovie(char *pszMovie);

/* rdata */

extern const int engine_inf;      // weak
extern const int rand_increment;  // unused
extern const int rand_multiplier; // unused

#endif /* __ENGINE_H__ */
