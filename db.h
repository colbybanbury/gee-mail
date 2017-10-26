#ifndef DB_H
#define DB_H

int init_db(void);
/*
  initialize databases if the do not exist, return exit code */


char** get_message_signatures(char* user, int len_u);
/*
  return pointer to array of message signatures for a user */


int get_message_count(char* user, int len_u);
/*
  return number of messages addressed to a user*/


char* get_message(int id);
/*
  return the message contents for a message with this id num*/


int send_message(char* sender, char* receiver, char* message, char* passphrase, \
                 int len_s, int len_r, int len_m, int len_p);
/*
  add message to db with given parameters and hashed passphrase */


int check_user(char* user, int len_u);
/*
 verify if given user exists in user table */


char* get_passcode(char* user, int len_u);
/*
 return hashed passcode for a given user */

int register_user(char* user, char* pass, int len_u, int len_p);
/*
  create a new user entry with given username and password */


#endif
