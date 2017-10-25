#include <stdio.h>

int main(void){
  char* name, pass, sel, message, passphrase, destination;
  init_db();
  printf("Press s to sign in. Press r to register.\n");
  gets(sel);
  if(sel=='r'){
    reg();
  }else{
    sign_in();
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
