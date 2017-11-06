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
  
  
  char* iniVector = malloc(9); // 9 bytes (8 for nonce and 1 for null term)
  gcry_create_nonce(iniVector, 8);
  iniVector[8] = 0;
  printf("%s\n", iniVector);


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


  char * encBuffer = malloc(len_m + 9);

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

  encBuffer[len_m] = 0;

  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  strcat(encBuffer, iniVector);	//concat nonce to end of encypted message

  printf("encBuffer + nonce = ");
  for(index=0;index<len_m+9;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  free(key);
  free(iniVector);
  return encBuffer;
  
}

char* unencrypt(char* message, char* passphrase){

  gcry_error_t gcryError;
  gcry_cipher_hd_t gcryCipherHd;
  size_t index,i;
  size_t len_p = strlen(passphrase) < 32 ? strlen(passphrase) : 32;
  size_t len_m = strlen(message) - 8;//length of message minus the nonce
  char* key = malloc(33);
  
  char* placeholder = "$%&'()*+,-./0123456789:;<=>?@ABC";
  memcpy(key, passphrase, len_p);
  for(i=0;i<32-len_p;i++){
    key[len_p+i] = placeholder[i];
  }
  key[32] = 0;
  
  printf("%s\n", key);
  
  
  char* iniVector = malloc(8); // 8 bytes (why?)
  for(i=0; i<8; i++){
  	iniVector[i] = message[len_m+i];
  }
  char* message_minus_nonce = malloc(len_m+1);
   for(i=0; i<len_m; i++){
  	message_minus_nonce[i] = message[i];
  }
  message_minus_nonce[len_m] = 0;

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

  char * encBuffer = malloc(len_m + 1); // Plus one to hold null-terminator

  gcryError = gcry_cipher_decrypt(gcryCipherHd,
                                  encBuffer,
                                  len_m,
                                  message_minus_nonce,
                                  len_m);
  if( gcryError ){
    printf("gcry_cipher_decrypt failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  printf("gcry_cipher_decrypt worked\n");

  encBuffer[len_m] = 0;

  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }

  printf("\n");
  free(key);
  free(iniVector);
  free(message_minus_nonce);
  return encBuffer;
  
}
