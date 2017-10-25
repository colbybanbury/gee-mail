#ifndef ENCR_H
#define ENCR_H

char* hash(char* pass, int len);
/*
  hash given character array, return result */

char* encrypt(char* message, char* passphrase, int len_m, int len_p);
/*
  encrypt given message of length len_m with passphrase of length len_p */

char* unencrypt(char* encr_message, char* passphrase, int len_m, int len_p);
/*
  unencrypt given encr_message of length len_m with passphrase of length 
  len_p */

#endif
