#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "db.h"
#include "encr.h"

int test_db(void);
int test_encr(void);

int main(void){
  test_db();
  test_encr();
  return 0;
}

int test_encr(void){
  char* tmp = bin_to_hex_char(0b10101111);
  assert('a' == tmp[0]);
  assert('f' == tmp[1]);
  tmp = bin_to_hex_char(0b00000000);
  assert('0' == tmp[0]);
  assert('0' == tmp[1]);
  tmp = bin_to_hex_char(0b01001100);
  assert('4' == tmp[0]);
  assert('c' == tmp[1]);

  assert(strcmp("2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824",
                hash_to_string(hash("hello"))) == 0);
  assert(strcmp("f6952d6eef555ddd87aca66e56b91530222d6e318414816f3ba7cf5bf694bf0f",
                hash_to_string(hash("penis"))) == 0);
  assert(strcmp("cc4c1c16512a1c5e62b007e041c6fa6e677ab2a5ae06f6833aeb7551a713ff9d",
                hash_to_string(hash("this was colby's fault"))) == 0);

  printf("hashes hashed successfully\n");
  
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

  assert(check_password(receiver, pass) == 1);
  assert(check_password(receiver, "wrongpass") == 0);

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
