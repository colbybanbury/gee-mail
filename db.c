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

static int check_user_callback(void *exists, int argc, char **argv, char **azColName){
  *(int*)exists = (int)argv[0][0] -48; //48 is ascii 0
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

int register_user(char* user, char* pass){
  int len_u = strlen(user);
  int len_p = strlen(pass);
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

int check_user(char* user){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open(DB_NAME, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 0;
  }
  int len_u = strlen(user);
  char* query = (char*)malloc((51 + len_u + 13));
  // example: "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAMsE = "user" LIMIT 1);"\0
  //          |----------51----------------------------------|--len_u--|----13-
  strcpy(query, "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAME = \"");
  strcat(query, user);
  strcat(query, "\" LIMIT 1);");

  printf("%s\n", query);

  int exists = 0;

  rc = sqlite3_exec(db, query, check_user_callback, &exists, &zErrMsg);

  printf("%d\n", exists);
  if(!exists){
    printf("user does not exist\n");
  }
  else{printf("user found\n");}
  sqlite3_close(db);
  free(query);
  return exists;
}

int send_message(char* sender, char* receiver, char* message, char* passphrase){
  int len_s = strlen(sender);
  int len_r = strlen(receiver);
  int len_m = strlen(message);
  int len_p = strlen(passphrase);
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

int count_results(void *data, int argc, char **argv, char **azColName){
  (*(int*)data)++;
  return 0;
}

int get_message_count(char* user){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open(DB_NAME, &db);

  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 1;
  }

  char* query = (char*)malloc(39 + strlen(user) + 3);
  // example: select * from messages where receiver="username";\0
  //          |-----------------39------------------|-len_u--|3-|

  strcpy(query, "select * from MESSAGES where receiver=\"");
  strcat(query, user);
  strcat(query, "\";");

  printf("%s\n", query);

  int num_messages = 0;
  rc = sqlite3_exec(db, query, count_results, &num_messages, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    fprintf(stdout, "Messages counted successfully\n");
  }
  printf("number of messages: %i\n", num_messages);
  free(query);
  sqlite3_close(db);  
  return num_messages;
  
}
