#ifndef VALDT_H
#define VALDT_H

int validate_user(char* user, char* pass);
/*
  validate that a user and password match */


int validate_message(int message_id, char* pass);
/*
  validate that a passphrase for a given message id hashes correctly */

#endif
