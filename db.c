#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "db.h"

#define DB_NAME "mail.db"

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
  int rc = sqlite3_open(DB_NAME, &db);
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

int register_user(char* user, char* pass, int len_u, int len_p){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open(DB_NAME, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char* query = (char*)malloc((26 + len_u + 4 + len_p + 5));
  // example: "insert into USERS values ("username", "pass");"\0
  //          |----------26-------------|--len_u--|4-|len_p|-5-|

  strcpy(query, "insert into USERS values (\"");
  strcat(query, user);
  strcat(query, "\", \"");
  strcat(query, pass);
  strcat(query, "\");");

  printf("%s\n", query);

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "User added successfully\n");
  } 

  sqlite3_close(db);
  free(query);
  return 0;
}

int send_message(char* sender, char* receiver, char* message, char* passphrase, \
                 int len_s, int len_r, int len_m, int len_p){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open(DB_NAME, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char* query = (char*)malloc((67 + 10 + len_s + 4 + len_r + 4 + len_m + 4 \
                               + len_p + 5));
  // example: "insert into MESSAGES ("sender", "receiver", "message", "passphrase") \
  //          |----------------------------67-------------------------------------|
  // values ("sender_var", "receiver_var", "message_var", "passphrase_var");"
  //|---10---|--len_s---|4-|--len_r-----|4-|---len_m---|4-|----len_p-----|5-|

  strcpy(query, "insert into MESSAGES (\"sender\", \"receiver\", \"message\", \
 \"passphrase\") values (\"");
  strcat(query, sender);
  strcat(query, "\", \"");
  strcat(query, receiver);
  strcat(query, "\", \"");
  strcat(query, message);
  strcat(query, "\", \"");
  strcat(query, passphrase);
  strcat(query, "\");");

  printf("%s\n", query);

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "Message sent successfully\n");
  }

  free(query);
  sqlite3_close(db);
  return 0;
  
}
