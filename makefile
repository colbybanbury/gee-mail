CC = gcc
CFLAGS = -lsqlite3
EXEC = mail
DB = mail.db
OBJS = db_init.o

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXEC) $(OBJS) $(DB)
