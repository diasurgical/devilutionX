//HEADER_GOES_HERE
#ifndef __SHA_H__
#define __SHA_H__

/*
 *  Define the SHA1 circular left shift macro
 */
#define SHA1CircularShift(bits,word) \
				(((word) << (bits)) | ((word) >> (32-(bits))))
#define SHA1HashSize 20

//sha
extern SHA1Context sgSHA1[3];

void __cdecl SHA1Clear();
void __fastcall SHA1Result(int n, char Message_Digest[SHA1HashSize]);
void __fastcall SHA1Calculate(int n, const char *data, char Message_Digest[SHA1HashSize]);
void __fastcall SHA1Input(SHA1Context *context, const char *message_array, int len);
void __fastcall SHA1ProcessMessageBlock(SHA1Context *context);
void __fastcall SHA1Reset(int n);

#endif /* __SHA_H__ */
