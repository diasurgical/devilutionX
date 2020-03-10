//HEADER_GOES_HERE
#ifndef __SHA_H__
#define __SHA_H__

#define SHA1HashSize 20

//sha

void SHA1Clear();
void SHA1Result(int n, char Message_Digest[SHA1HashSize]);
void SHA1Calculate(int n, const char *data, char Message_Digest[SHA1HashSize]);
void SHA1Input(SHA1Context *context, const char *message_array, int len);
void SHA1ProcessMessageBlock(SHA1Context *context);
void SHA1Reset(int n);
void SHA1Init(SHA1Context *context);

#endif /* __SHA_H__ */
