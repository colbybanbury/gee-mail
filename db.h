#ifndef DB_H
#define DB_H

int init_db(int is_test);
/*
  if is_test==1 then test_mail.db is initialized. Otherwise it's mail.db
  initialize databases if the do not exist, return exit code */


char*** get_message_signatures(char* user);
/*
  return pointer to array of message signatures for a user */


int get_message_count(char* user);
/*
  return number of messages addressed to a user*/


char* get_message(int id);
/*
  return the message contents for a message with this id num*/


int send_message(char* sender, char* receiver, char* message, char* passphrase);
/*
  add message to db with given parameters and hashed passphrase */


int check_user(char* user);
/*
 verify if given user exists in user table */

int check_password(char* username, char* password);
/*
 verfiy that the given username and password match in the database*/
char* get_passcode(char* user);
/*
 return hashed passcode for a given user */

int register_user(char* user, char* pass);
/*
  create a new user entry with given username and password */


#endif
