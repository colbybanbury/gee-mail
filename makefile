CC = gcc
CFLAGS = -g
CLINKS = -lsqlite3
EXEC = mail
DB = mail.db
OBJS = db.o
MAIN = mail.o
TEST = test.o

$(EXEC): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) $(CLINKS) -o $@ $^

clean:
	rm $(EXEC) $(OBJS) $(DB) $(TEST)

test: $(OBJS) $(TEST)
	$(CC) $(CFLAGS) $(CLINKS) -o $@ $^
