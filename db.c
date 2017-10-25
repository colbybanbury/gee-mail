#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i]:"NULL");
  }
  printf("\n");
  return 0;
}

int init_db(void){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open("mail.db", &db);
  rc = sqlite3_open("mail.db", &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char* queries[] = {"create table users(USERNAME text primary key, \
PASSWORD text);",
                     "create table messages(\
ID integer primary key autoincrement, SENDER text, RECEIVER text, \
MESSAGE text, PASSPHRASE text);"
  };

  int i;
  for(i=0;i<sizeof(queries)/sizeof(char*);i++){
    rc = sqlite3_exec(db, queries[i], callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
      fprintf(stdout, "Table created successfully\n");
    }
  }
  sqlite3_close(db);
    
  return 0;
}

