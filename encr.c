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

  #ifdef DEBUG
  printf("hashBuffer = ");
  for( i=0;i<32;i++ ){
        printf("%02X", (unsigned char)hashBuffer[i]);
  }
  printf("\n");
  #endif

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
  #ifdef DEBUG
  printf("%s\n",dest);
  #endif

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
  
  #ifdef DEBUG
  printf("%s\n", key);
  #endif
  
  
  char* nonce = malloc(9); // 9 bytes (8 for nonce and 1 for null term)
  gcry_create_nonce(nonce, 8);
  printf("nonce is ");
  for(i = 0; i <8; i++){
  	printf("%02X", (unsigned char)nonce[i]);
  	if(nonce[i] == 0){
  		nonce[i] = 2;
  	}
  }
  nonce[8] = 0;
  printf("\n");

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
  #ifdef DEBUG
  printf("gcry_cipher_open worked\n");
  #endif

  gcryError = gcry_cipher_setkey(gcryCipherHd, key, 32);
  if( gcryError ){
    printf("gcry_cipher_setkey failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  #ifdef DEBUG
  printf("gcry_cipher_setkey worked\n");
  #endif

  gcryError = gcry_cipher_setiv(gcryCipherHd, nonce, 8);
  if( gcryError ){
    printf("gcry_cipher_setiv failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  #ifdef DEBUG
  printf("gcry_cipher_setiv worked\n");
  #endif


  char * encBuffer = malloc(len_m +1);

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
  #ifdef DEBUG
  printf("gcry_cipher_encrypt worked\n");
  #endif

  encBuffer[len_m] = 0;

  #ifdef DEBUG
  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  #endif

  char* encMessage = malloc(len_m+11);

  encMessage[0] = len_m/127 + 1;
  encMessage[1] = len_m%127;

  for(i = 0; i< 8; i++){
    encMessage[i+2] = nonce[i];
  }
  for(i = 0; i< len_m; i++){
    encMessage[i+10] = encBuffer[i];
  }
  //concat nonce to beginning of encypted message
  encMessage[len_m + 10] = 0;

  #ifdef DEBUG
  printf("size + nonce + encBuffer = ");
  for(index=0;index<len_m+10;index++){
    printf("%02X", (unsigned char)encMessage[index]);
  }
  printf("\n");
  #endif

  free(key);
  free(nonce);
  return encMessage;
  
}

char* unencrypt(char* message, char* passphrase){

  gcry_error_t gcryError;
  gcry_cipher_hd_t gcryCipherHd;
  size_t index,i;
  size_t len_p = strlen(passphrase) < 32 ? strlen(passphrase) : 32;
  size_t len_m = (message[0] -1)* 127 + message[1];//length of message minus the nonce
  char* key = malloc(33);
  
  char* placeholder = "$%&'()*+,-./0123456789:;<=>?@ABC";
  memcpy(key, passphrase, len_p);
  for(i=0;i<32-len_p;i++){
    key[len_p+i] = placeholder[i];
  }
  key[32] = 0;
  
  #ifdef DEBUG
  printf("%s\n", key);
  #endif
  
  
  char* nonce = malloc(9); // 8 bytes for nonce and 1 for null term(why?)
  for(i=0; i<8; i++){
  	nonce[i] = message[i+2];
  }
  nonce[8] = 0;
  char* message_minus_nonce = malloc(len_m+1);
   for(i=0; i<len_m; i++){
  	message_minus_nonce[i] = message[i+10];
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
  #ifdef DEBUG
  printf("gcry_cipher_open worked\n");
  #endif

  gcryError = gcry_cipher_setkey(gcryCipherHd, key, 32);
  if( gcryError ){
    printf("gcry_cipher_setkey failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strsource(gcryError));
    return 0;
  }
  #ifdef DEBUG
  printf("gcry_cipher_setkey worked\n");
  #endif

  gcryError = gcry_cipher_setiv(gcryCipherHd, nonce, 8);
  if( gcryError ){
    printf("gcry_cipher_setiv failed:  %s/%s\n",
           gcry_strsource(gcryError),
           gcry_strerror(gcryError));
    return 0;
  }
  #ifdef DEBUG
  printf("gcry_cipher_setiv worked\n");
  #endif

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
  #ifdef DEBUG
  printf("gcry_cipher_decrypt worked\n");
  #endif

  encBuffer[len_m] = 0;

  #ifdef DEBUG
  printf("encBuffer = ");
  for(index=0;index<len_m;index++){
    printf("%02X", (unsigned char)encBuffer[index]);
  }
  printf("\n");
  printf("%s\n", encBuffer);
  #endif
  free(key);
  free(nonce);
  free(message_minus_nonce);
  return encBuffer;
  
}
