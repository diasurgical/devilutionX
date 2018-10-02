//HEADER_GOES_HERE
#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

extern int hashtable[1280];

void __fastcall Decrypt(void *block, int size, int key);
void __fastcall Encrypt(void *block, int size, int key);
int __fastcall Hash(const char *s, int type);
void __cdecl InitHash();
int __fastcall PkwareCompress(void *buf, int size);
unsigned int __cdecl PkwareBufferRead(char *buf, unsigned int *size, void *param);
void __cdecl PkwareBufferWrite(char *buf, unsigned int *size, void *param);
void __fastcall PkwareDecompress(void *param, int recv_size, int dwMaxBytes);

#endif /* __ENCRYPT_H__ */
