#ifndef ENCR_H
#define ENCR_H

char* hash(char* pass);
/*
  hash given character array, return result */

char* encrypt(char* message, char* passphrase);
/*
  encrypt given message of length len_m with passphrase of length len_p */

char* unencrypt(char* encr_message, char* passphrase);
/*
  unencrypt given encr_message of length len_m with passphrase of length 
  len_p */

char* bin_to_hex_char(char input);
/*
  convert a single byte to two ascii characters
 */
char* hash_to_string(char* hash);
/*
  convert a hash from binary byte-wise representation to ascii hex 
 */
#endif
