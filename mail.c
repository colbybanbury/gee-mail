#include <stdio.h>
#include "db.h"
#include "encr.h"
#include "valdt.h"
#include "mail.h"


char* reg(){
  char *username = (char *) malloc(sizeof(char) * 50);
  char *password = (char *) malloc(sizeof(char) * 50);
  int usernameSize = sizeof(username);
  int passwordSize = sizeof(password);
  printf("enter a new username:");
  getline(&username, &usernameSize, stdin);
  username[strcspn(username, "\n")] = 0; //remove newline

  while(check_user(username)){
      printf("that username is already in use\n");
      printf("enter a new username or type s to sign into an existing account:");
      getline(&username, &usernameSize, stdin);
      username[strcspn(username, "\n")] = 0; //remove newline
      if(strcmp(username, "s") == 0){
        return sign_in();
      }
  }
  printf("enter a new password:");
  getline(&password, &passwordSize, stdin);

  password[strcspn(password, "\n")] = 0; //remove newline

  
  if(register_user(username, hash(password)) == 0)
  {
    printf("Successfully registered. Welcome!");
  }
  memset(password, '0', 50);
  return username;
}

char* sign_in(){
  char *username = (char *) malloc(sizeof(char) * 50);
  char *password = (char *) malloc(sizeof(char) * 50);
  int usernameSize = sizeof(username);
  int passwordSize = sizeof(password);
  printf("enter your username:");
  getline(&username, &usernameSize, stdin);
  username[strcspn(username, "\n")] = 0; //remove newline

  while(!check_user(username)){
      printf("no matching username\n");
      printf("enter your username or type r to register:");
      getline(&username, &usernameSize, stdin);
      username[strcspn(username, "\n")] = 0; //remove newline
      if(strcmp(username, "r") == 0){
        return reg();
      }
  }
  printf("enter your password:");

  getline(&password, &passwordSize, stdin);
  password[strcspn(password, "\n")] = 0; //remove newline

  while(!check_password(username, hash(password))){
    printf("incorrect password\n");
    printf("enter your password:");
    getline(&password, &passwordSize, stdin);
    password[strcspn(password, "\n")] = 0; //remove newline
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
  char* name;
  char* pass;
  char* sel;
  char* message;
  char* passphrase;
  char* destination;
  char* idString;

  init_db(0);
  printf("Press s to sign in. Press r to register.\n");

  sel = (char*) malloc(1);
  int selSize = sizeof(sel);
  getline(&sel, &selSize, stdin);

  sel[strcspn(sel, "\n")] = 0;
  
  if(strcmp(sel,"r") == 0){
    name = reg();
  }else{
    name = sign_in();
  }
  disp_messages(name);
  printf("r to read, w to write, or q to quit\n");
  getline(&sel, &selSize, stdin);
  sel[strcspn(sel, "\n")] = 0;

  passphrase = (char*) malloc (sizeof(char) * 20);
  destination = (char*) malloc (sizeof(char) * 20);
  message = (char*) malloc (sizeof(char) * 120);

  idString = (char*) malloc (sizeof(char) * 20);

  int passphraseSize = sizeof(passphrase);
  int destinationSize = sizeof(destination);
  int messageSize = sizeof(message);
  int idStringSize = sizeof(idString);

  while(strcmp(sel, "q") != 0){
    if((strcmp(sel, "r")== 0)) {
      printf("Enter a message ID: ");

      getline(&idString, &idStringSize, stdin);
      idString[strcspn(idString, "\n")] = 0;

      int id = atoi(idString);

      printf("Enter a passphrase: ");
      getline(&passphrase, &passphraseSize, stdin);
      passphrase[strcspn(passphrase, "\n")] = 0;

      printf("\n%s\n", get_message(name, id, passphrase));
    }else{

      printf("Enter user to send message to: ");
      getline(&destination, &destinationSize, stdin);
      destination[strcspn(destination, "\n")] = 0;

      while(!check_user(destination)){
        printf("No matching username\n");
        getline(&destination, &destinationSize, stdin);
        destination[strcspn(destination, "\n")] = 0;
      }

      printf("Enter message: ");
      getline(&message, &messageSize, stdin);
      message[strcspn(message, "\n")] = 0;

      printf("Enter passphrase: ");
      getline(&passphrase, &passphraseSize, stdin);
      passphrase[strcspn(passphrase, "\n")] = 0;

      message = encrypt(message,passphrase);
      passphrase = hash(passphrase);
      send_message(name, destination, message, passphrase);
    }
    disp_messages(name);
    printf("r to read, w to write, or q to quit\n");

    getline(&sel, &selSize, stdin);
    sel[strcspn(sel, "\n")] = 0;

  }
    
  return 0;
}

char* getUserInput(){

}