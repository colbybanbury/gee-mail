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

  printf("hashBuffer = ");
  for( i=0;i<32;i++ ){
    printf("%02X", (unsigned char)hashBuffer[i]);
  }
  printf("\n");
  return hashBuffer;
}

char* bin_to_hex_char(char input){
  char * ptr = malloc(2);
    
  ptr[0] = (input >> 4) & 0x0f;
  ptr[1] = input & 0x0f;
  int i;
  for(i=0;i<2;i++){
    if(ptr[i] < 10) ptr[i] += 48;
    else ptr[i] += (97-10);
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
  printf("%s\n",dest);
  return dest;
}
