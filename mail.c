#include <stdio.h>
#include "db.h"
#include "encr.h"
#include "valdt.h"

int main(void){
  char* name, pass, sel, message, passphrase, destination;
  init_db();
  printf("Press s to sign in. Press r to register.\n");
  gets(sel);
  if(sel=='r'){
    reg();
  }else{
    sign_in(name, pass);
  }
  disp_messages();
  printf("r to read, w to write\n");
  gets(sel);
  if(sel == 'r'){
    printf("enter a message ID\n");
    int id;
    gets(id);
    gets(passphrase);
    if(validate(id, passphrase)){
      message = decode(id, passphrase);
      printf("%s\n",message);
    }else{
      printf("invalid passphrase\n");
    }
  }else{
    gets(destination);
    while(!check_user(destination)){
      printf("no matching username\n");
      gets(destination);
    }
    gets(message);
    gets(passphrase);
    message = encrypt(message,passphrase);
    passphrase = hash(passphrase);
    send_message(message,passphrase);
  }
    
  return 0;
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
        reg();
        return 0;
      }
  }
  printf("enter your password:");
  fgets(password, 50, stdin);
  while(!check_password(username, hash(password))){
    printf("incorrect password\n");
    printf("enter your password:");
    fgets(username, 50, stdin);
  }
  memset(password, '0', 50)
  return username;
}
