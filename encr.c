#include <stdio.h>
#include <gcrypt.h>
#include <string.h>
#include "encr.h"

char* hash(char* pass){
  size_t i;
  size_t txtLength = strlen(pass);
  
  char* hashBuffer = malloc(33);
  memset(hashBuffer, 0, 33);

  gcry_md_hash_buffer(
      GCRY_MD_SHA256,
      hashBuffer,
      pass,
      txtLength);

  //  printf("hashBuffer = ");
  for( i=0;i<32;i++ ){
    //    printf("%02X", (unsigned char)hashBuffer[i]);
  }
  //  printf("\n");
  return hashBuffer;
}

char* bin_to_hex_char(char input){
  char * ptr = malloc(2);
    
  ptr[0] = (input >> 4) & 0x0f;
  ptr[1] = input & 0x0f;
  int i;
  for(i=0;i<2;i++){
    ptr[i] = ptr[i] < 10 ? ptr[i] + 48 : ptr[i] + 87;
    // convert numerical value to ascii hex (0-9 -> +48; a-f -> +87)
  }

  return ptr; 
  
}

char* hash_to_string(char* hash){
  int len = strlen(hash);
  int i;
  char* dest = (char*)malloc(len*2+1);
  char* result;
  for(i=0;i<len;i++){
    result = bin_to_hex_char(hash[i]);
    memcpy(dest+(2*i), result, 2);
  }
  dest[len*2] = 0;
  //  printf("%s\n",dest);
  return dest;
}

char* encrypt(char* message, char* passphrase){
  gcry_error_t gcryError;
  gcry_cipher_hd_t gcryCipherHd;
  size_t index,i;
  size_t len_p = strlen(passphrase);
  size_t len_m = strlen(message);
  char* key = malloc(33);

  char* placeholder = "$%&'()*+,-./0123456789:;<=>?@ABC";
  memcpy(key, passphrase, len_p);
  for(i=0;i<32-len_p;i++){
    key[len_p+i] = placeholder[i];
  }
  key[32] = 0;
  
  printf("%s\n", key);
  
  
  char* iniVector = "AAAAAAAA"; // 8 bytes (why?)

  gcryError = gcry_cipher_open(&gcryCipherHd,
                               GCRY_CIPHER_SALSA20,
                               GCRY_CIPHER_MODE_STREAM,
                               0);

  if( gcryError ){
    printf("gcry_cipher_open failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  printf("gcry_cipher_open worked\n");

  gcryError = gcry_cipher_setkey(gcryCipherHd, key, 32);
  if( gcryError ){
    printf("gcry_cipher_setkey failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  printf("gcry_cipher_setkey worked\n");

  gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, 8);
  if( gcryError ){
    printf("gcry_cipher_setiv failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  printf("gcry_cipher_setiv worked\n");

  char * encBuffer = malloc(len_m);

  gcryError = gcry_cipher_encrypt(gcryCipherHd,
                                  encBuffer,
                                  len_m,
                                  message,
                                  len_m);
  if( gcryError ){
    printf("gcry_cipher_encrypt failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  printf("gcry_cipher_encrypt worked\n");

  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  return encBuffer;
  
}

char* unencrypt(char* message, char* passphrase){

  gcry_error_t gcryError;
  gcry_cipher_hd_t gcryCipherHd;
  size_t index,i;
  size_t len_p = strlen(passphrase) < 32 ? strlen(passphrase) : 32;
  size_t len_m = strlen(message);
  char* key = malloc(33);
  
  char* placeholder = "$%&'()*+,-./0123456789:;<=>?@ABC";
  memcpy(key, passphrase, len_p);
  for(i=0;i<32-len_p;i++){
    key[len_p+i] = placeholder[i];
  }
  key[32] = 0;
  
  printf("%s\n", key);
  
  
  char* iniVector = "AAAAAAAA"; // 8 bytes (why?)

  gcryError = gcry_cipher_open(&gcryCipherHd,
                               GCRY_CIPHER_SALSA20,
                               GCRY_CIPHER_MODE_STREAM,
                               0);

  if( gcryError ){
    printf("gcry_cipher_open failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  printf("gcry_cipher_open worked\n");

  gcryError = gcry_cipher_setkey(gcryCipherHd, key, 32);
  if( gcryError ){
    printf("gcry_cipher_setkey failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  printf("gcry_cipher_setkey worked\n");

  gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, 8);
  if( gcryError ){
    printf("gcry_cipher_setiv failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  printf("gcry_cipher_setiv worked\n");

  char * encBuffer = malloc(len_m);

  gcryError = gcry_cipher_decrypt(gcryCipherHd,
                                  encBuffer,
                                  len_m,
                                  message,
                                  len_m);
  if( gcryError ){
    printf("gcry_cipher_decrypt failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  printf("gcry_cipher_decrypt worked\n");

  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  return encBuffer;
  
}
