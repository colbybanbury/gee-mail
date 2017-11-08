#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "db.h"
#include "encr.h"

char* DB_NAME = "mail.db";

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

static int check_password_callback(void *exists, int argc, char **argv, char **azColName){
  *(int*)exists = (int)argv[0][0] -48; //48 is ascii 0
  return 0;
}

int init_db(int is_test){//if is_test == 1 then initializes to a different db
  sqlite3* db;
  char *zErrMsg = 0;
  if(is_test==1){
    DB_NAME = "test_mail.db";
  }
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
      #ifdef DEBUG
      fprintf(stdout, "Table created successfully\n");
      #endif
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
  strncat(query, user, len_u);
  strncat(query, "\", \"", 4);
  strncat(query, pass, len_p);
  strncat(query, "\");", 5);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    #ifdef DEBUG
    fprintf(stdout, "User added successfully\n");
    #endif
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
  strncat(query, user, len_u);
  strncat(query, "\" LIMIT 1);", 13);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  int exists = 0;

  rc = sqlite3_exec(db, query, check_user_callback, &exists, &zErrMsg);

  #ifdef DEBUG
  printf("%d\n", exists);
  if(!exists){
    printf("user does not exist\n");
  }
  else{printf("user found\n");}
  #endif

  sqlite3_close(db);
  free(query);
  return exists;
}

int check_password(char* username, char* password){
  sqlite3* db;
  char *zErrMsg = 0;
  int rc = sqlite3_open(DB_NAME, &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return 0;
  }
  int len_u = strlen(username);
  int len_p = strlen(password);
  char* query = (char*)malloc((51 + len_u + 18 + len_p + 12));
  // example: "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAMsE = "user" AND PASSWORD = "pasword" LIMIT 1);"\0
  //          |----------51--------------------------------------|--len_u--|---18--------|--len_p|--12---
  strcpy(query, "SELECT EXISTS(SELECT 1 FROM USERS WHERE USERNAME = \"");
  strncat(query, username, len_u);
  strncat(query, "\" AND PASSWORD = \"", 18);
  strncat(query, password, len_p);
  strncat(query, "\"LIMIT 1);", 12);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  int exists = 0;

  rc = sqlite3_exec(db, query, check_password_callback, &exists, &zErrMsg);

  #ifdef DEBUG
  printf("%d\n", exists);
  if(!exists){
    printf("user does not exist\n");
  }
  else{printf("user found\n");}
  #endif

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
  strncat(query, sender, len_s);
  strncat(query, "\", \"", 4);
  strncat(query, receiver, len_r);
  strncat(query, "\", \"", 4);
  strncat(query, message, len_m);
  strncat(query, "\", \"", 4);
  strncat(query, passphrase, len_p);
  strncat(query, "\");", 5);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    #ifdef DEBUG
    fprintf(stdout, "Message sent successfully\n");
    #endif
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

  size_t len_u = strlen(user);
  
  char* query = (char*)malloc(39 + len_u + 3);
  // example: select * from messages where receiver="username";\0
  //          |-----------------39------------------|-len_u--|3-|

  strcpy(query, "select * from MESSAGES where receiver=\"");
  strncat(query, user, len_u);
  strncat(query, "\";", 3);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  int num_messages = 0;
  rc = sqlite3_exec(db, query, count_results, &num_messages, &zErrMsg);
  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    #ifdef DEBUG
    fprintf(stdout, "Messages counted successfully\n");
    #endif
  }

  #ifdef DEBUG
  printf("number of messages: %i\n", num_messages);
  #endif

  free(query);
  sqlite3_close(db);  
  return num_messages;
  
}

char*** get_message_signatures(char* user){
  sqlite3* db;
  char *zErrMsg = 0;
  sqlite3_stmt *res;
  int rc = sqlite3_open(DB_NAME, &db);

  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    
    return (char***)1;
  }

  size_t len_u = strlen(user);
  
  char* query = (char*)malloc(39 + len_u + 3);
  // example: select * from messages where receiver="username";\0
  //          |-----------------39------------------|-len_u--|3-|

  strcpy(query, "select * from MESSAGES where receiver=\"");
  strncat(query, user, len_u);
  strncat(query, "\";", 3);

  #ifdef DEBUG
  printf("%s\n", query);
  #endif

  int num_messages = get_message_count(user);
  char*** messages = (char***)malloc(num_messages*sizeof(char**));
  rc = sqlite3_prepare_v2(db, query, -1, &res, 0);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return (char***)1;
  }
  int i=0;
  do{
    rc = sqlite3_step(res);
    messages[i] = (char**)malloc(sqlite3_column_count(res)*sizeof(char*));
    int j;
    size_t field_size;
    for(j=0;j<sqlite3_column_count(res);j++){
      field_size = sqlite3_column_bytes(res, j); // calculate size of entry
      messages[i][j] = (char*)malloc(field_size+1); // allocate space in result arr
      memcpy(messages[i][j], sqlite3_column_text(res,j), field_size);
      // copy message to result arr
      messages[i][j][field_size] = 0;
      // add null term
      #ifdef DEBUG
      printf("%s\t", messages[i][j]);
      #endif      
      
    }

    #ifdef DEBUG
    printf("\n");
    #endif

    i++;
  } while( rc == SQLITE_ROW );
    
  sqlite3_finalize(res);
  sqlite3_close(db);
  return messages;
}
