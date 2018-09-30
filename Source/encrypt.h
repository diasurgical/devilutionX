//HEADER_GOES_HERE
#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

extern int encrypt_table[1280];
//int encrypt_52B564[257];

void __fastcall encrypt_decrypt_block(void *block, int size, int key);
void __fastcall encrypt_encrypt_block(void *block, int size, int key);
int __fastcall encrypt_hash(char *s, int type);
void __cdecl encrypt_init_lookup_table();
int __fastcall encrypt_compress(void *buf, int size);
unsigned int __cdecl encrypt_pkware_read(char *buf, unsigned int *size, void *param);
void __cdecl encrypt_pkware_write(char *buf, unsigned int *size, void *param);
void __fastcall encrypt_decompress(void *param, int recv_size, int dwMaxBytes);

#endif /* __ENCRYPT_H__ */
