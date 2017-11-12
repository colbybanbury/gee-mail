#ifndef MAIL_H
#define MAIL_H

char* sign_in();
/*
    Prompt the user to sign in, check database for validity to sign in */

char* reg();
/*
    Prompt the user to enter a unique username and password, and add entry to db */

void disp_messages(char* name);
/*
    Display all messages that the current user can open. */

void getUserInput(char* result, int size);

#endif