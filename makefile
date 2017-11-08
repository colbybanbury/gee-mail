CC = gcc
CFLAGS = -g -D DEBUG
CLINKS = -lsqlite3 -lgcrypt
EXEC = mail
DB = test_mail.db
OBJS = db.o encr.o
MAIN = mail.o
TEST = test.o

$(EXEC): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $^ $(CLINKS)

clean:
	rm $(EXEC) $(OBJS) $(DB) $(TEST)

test: $(OBJS) $(TEST)
	$(CC) $(CFLAGS) -o $@ $^ $(CLINKS)

