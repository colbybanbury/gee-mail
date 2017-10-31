#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "db.h"

int test_db(void);

int main(void){
  test_db();
  return 0;
}

int test_db(void){
  if(remove("mail.db")){
    printf("File does not exist \n");
  }
  char* sender = "Collin";
  char* receiver = "Colby";
  char* hash = "somehash";
  char* pass = "somepass";
  char* message1 = "write more tests";
  char* message2 = "make this compile";
  assert(init_db() == 0);

  assert(register_user(receiver, pass) == 0);
  assert(register_user(sender, pass) == 0);

  assert(check_user("Colby") == 1);
  assert(check_user("Dave") == 0);

  assert(send_message(sender, receiver, message1, hash) == 0);
  assert(get_message_count(receiver) == 1);
  assert(get_message_count(sender) == 0);
  assert(send_message(sender, receiver, message2, hash) == 0);
  assert(get_message_count(receiver) == 2);
  assert(get_message_count(sender) == 0);
    
  char*** messages_actual = get_message_signatures(receiver);
  char* temp1[] = {"1", sender, receiver, message1, hash};
  char* temp2[] = {"2", sender, receiver, message2, hash};
  char** messages_expected[] = {temp1, temp2};
  int i,j;

  for(i=0;i<2;i++){
    for(j=0;j<5;j++){
      assert(strcmp(messages_actual[i][j], messages_expected[i][j]) == 0);
    }
  }
  printf("Messages retrieved successfully\n");
  
  return 0;
}
