#ifndef DB_H
#define DB_H

int init_db(void);
/*
  initialize databases if the do not exist, return exit code */


char** get_message_signatures(char* user);
/*
  return pointer to array of message signatures for a user */


int get_message_count(char* user);
/*
  return number of messages addressed to a user*/


char* get_message(int id);
/*
  return the message contents for a message with this id num*/


int send_message(char* message, char* passphrase, char* sender, char* receiver);
/*
  add message to db with given parameters and hashed passphrase */


int check_user(char* user);
/*
 verify if given user exists in user table */


char* get_passcode(char* user);
/*
 return hashed passcode for a given user */



#endif
