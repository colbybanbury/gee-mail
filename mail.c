#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "encr.h"
#include "valdt.h"
#include "mail.h"


char* reg(){
  char *username = (char *) malloc(sizeof(char) * 50);
  char *password = (char *) malloc(sizeof(char) * 50);
  int usernameSize = sizeof(char) * 50;
  int passwordSize = sizeof(char) * 50;
  printf("enter a new username:\n");
  
  getUserInput(username, usernameSize);

  while(check_user(username)){
      printf("that username is already in use\n");
      printf("enter a new username or type s to sign into an existing account:\n");
      getUserInput(username, usernameSize);
      if(strcmp(username, "s") == 0){
        return sign_in();
      }
  }
  printf("enter a new password:\n");
  getUserInput(password, passwordSize);

  if(register_user(username, hash(password)) == 0)
  {
    printf("Successfully registered. Welcome!\n");
  }
  memset(password, '0', 50);
  return username;
}

char* sign_in(){
  char *username = (char *) malloc(sizeof(char) * 50);
  char *password = (char *) malloc(sizeof(char) * 50);
  int usernameSize = sizeof(char) * 50;
  int passwordSize = sizeof(char) * 50;
  printf("Enter your username:\n");
  getUserInput(username, usernameSize);


  while(!check_user(username)){
      printf("No matching username\n");
      printf("Enter your username or type r to register:\n");
      getUserInput(username, usernameSize);
      if(strcmp(username, "r") == 0){
        return reg();
      }
  }
  printf("Enter your password:\n");

  getUserInput(password, passwordSize);

  while(!check_password(username, hash(password))){
    printf("incorrect password\n");
    printf("Try again:\n");
    getUserInput(password, passwordSize);
  }
  memset(password, '0', 50);
  return username;
}

void disp_messages(char* name){
  //find number of messages
  int messageCount = get_message_count(name);

  char*** messages = get_message_signatures(name);

  for(int i = 0; i < messageCount; i++){
    for(int k = 0; k < 2; k++){
      printf("%s ",messages[i][k]);
    }
    printf("\n");
  }
}

int main(void){
  char *name, *pass, *sel, *message, *passphrase, *destination, *idString;
  
  init_db(0);
  printf("Press s to sign in, r to register, or q to quit.\n");

  sel = (char*) malloc(2);
  int selSize = sizeof(char) * 2;
  
  while(1){
    getUserInput(sel, selSize);
    if(strcmp(sel,"r") == 0){
      name = reg();
      break;
    }else if (strcmp(sel, "s") == 0){
      name = sign_in();
      break;
    }else if (strcmp(sel, "q") == 0){
      return 0;
    }else{
      printf("Invalid input, please enter again:\n");
    }
  }

  passphrase = (char*) malloc (sizeof(char) * 20);
  destination = (char*) malloc (sizeof(char) * 20);
  message = (char*) malloc (sizeof(char) * 120);

  idString = (char*) malloc (sizeof(char) * 20);

  int passphraseSize = sizeof(char) * 20;
  int destinationSize = sizeof(char) * 20;
  int messageSize = sizeof(char) * 20;
  int idStringSize = sizeof(char) * 20;

  while(1){
    printf("=== Messages ===\n");
    disp_messages(name);
    printf("================\n");

    printf("r to read, w to write, or q to quit\n");
    getUserInput(sel, selSize);

    if((strcmp(sel, "r")== 0)) {
      printf("Enter a message ID:\n");

      getUserInput(idString, idStringSize);

      int id = atoi(idString);

      printf("Enter a passphrase:\n");
      getUserInput(passphrase, passphraseSize);

      printf("\nMessage:\n%s\n\n", get_message(name, id, passphrase));
    }else if(strcmp(sel, "w") == 0){

      printf("Enter user to send message to:\n");
      getUserInput(destination, destinationSize);

      while(!check_user(destination)){
        printf("No matching username\n");
        getUserInput(destination, destinationSize);
      }

      printf("Enter message:\n");
      getUserInput(message, messageSize);

      printf("Enter passphrase:\n");
      getUserInput(passphrase, passphraseSize);

      message = encrypt(message,passphrase);
      passphrase = hash(passphrase);
      send_message(name, destination, message, passphrase);
    }else if(strcmp(sel, "q") == 0){
      return 0;
    }else{
      printf("Invalid input, try again.\n");
    }
  }
  
    
  return 0;
}

//returns a correctly sized return string
void getUserInput(char* result, int size){
  printf("Enter %d character(s) maximimum. Any additional characters will be ignored.\n", size - 1); // -1 to ignore the newline character
  fgets(result, size, stdin);
  result[strcspn(result, "\n")] = 0;
  //printf("%d", strlen(result));
  if(strlen(result) == size - 1){ //only clear input buffer when input was too long
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { } //clear stdin buffer
  }
}