#include <stdio.h>
#include <assert.h>
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
  assert(init_db() == 0);
  assert(register_user("Colby", "somehash") == 0);
  assert(register_user("Collin", "otherhash") == 0);
  assert(send_message("Collin", "Colby", "write more tests", "a hash") == 0);
  assert(get_message_count("Colby") == 1);
  assert(get_message_count("Collin") == 0);
  assert(send_message("Collin", "Colby", "make this compile", "a hash") == 0);
  assert(get_message_count("Colby") == 2);
  assert(get_message_count("Collin") == 0);
  assert(check_user("Colby") == 1);
  assert(check_user("Dave") == 0);
  
  return 0;
}
