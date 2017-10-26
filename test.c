#include <stdio.h>
#include "db.h"

int main(void){
  init_db();
  register_user("hello", "null", 5, 4);
  send_message("Collin", "Colby", "write more tests", "testtest", 6, 5, 16, 8);
  return 0;
}
