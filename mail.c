#include <stdio.h>
#include "db.h"
#include "encr.h"
#include "valdt.h"

char*reg();
char* sign_in();

char* reg(){
  char username[50];
  char password[50];
  printf("enter a new username:");
  fgets(username, 50, stdin);
  while(check_user(username)){
      printf("that username is already in use\n");
      printf("enter a new username or type s to sign into an existing account:");
      fgets(username, 50, stdin);
      if(username == 's'){
        return sign_in();
      }
  }
  printf("enter a new password:");
  fgets(password, 50, stdin);
  
  if(register_user(username, hash(password)) == 0)
  {
    printf("Successfully registered. Welcome!");
  }
  memset(password, '0', 50);
  return username;
}

char* sign_in(){
  char username[50];
  char password[50];
  printf("enter your username:");
  fgets(username, 50, stdin);
  while(!check_user(username)){
      printf("no matching username\n");
      printf("enter your username or type r to register:");
      fgets(username, 50, stdin);
      if(username == 'r'){
        return reg();
      }
  }
  printf("enter your password:");
  fgets(password, 50, stdin);
  while(!check_password(username, hash(password))){
    printf("incorrect password\n");
    printf("enter your password:");
    fgets(username, 50, stdin);
  }
  memset(password, '0', 50);
  return username;
}


int main(void){
  char* name, pass, sel, message, passphrase, destination;
  init_db(0);
  printf("Press s to sign in. Press r to register.\n");
  fgets(sel, 1, stdin);
  if(sel=='r'){
    name = reg();
  }else{
    name = sign_in();
  }
  
  disp_messages();
  printf("r to read, w to write\n");
  fgets(sel, 1, stdin);
  if(sel == 'r'){
    printf("enter a message ID\n");
    int id;
    fgets(id, 20, stdin);
    fgets(passphrase, 20, stdin);
    if(validate(id, passphrase)){
      message = decode(id, passphrase);
      printf("%s\n",message);
    }else{
      printf("invalid passphrase\n");
    }
  }else{
    fgets(destination, 20, stdin);
    while(!check_user(destination)){
      printf("no matching username\n");
      fgets(destination, 20, stdin);
    }
    fgets(message, 140, stdin);
    fgets(passphrase, 20, stdin);
    message = encrypt(message,passphrase);
    passphrase = hash(passphrase);
    send_message(name, destination, message, passphrase);
  }
    
  return 0;
}