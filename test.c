#include <stdio.h>
#include "db.h"

int main(void){
  init_db();
  register_user("hello", "null");
  send_message("Collin", "Colby", "write more tests", "testtest");
  return 0;
}
