#include <stdio.h>
#include "db.h"

int main(void){
  init_db();
  register_user("hello", "null", 5, 4);

  return 0;
}
