CC = gcc
CFLAGS = -g
CLINKS = -lsqlite3
EXEC = mail
DB = mail.db
OBJS = db.o
MAIN = mail.o
TEST = test.o

$(EXEC): $(OBJS) $(MAIN)
	$(CC) $(CFLAGS) -o $@ $^ $(CLINKS)

clean:
	rm $(EXEC) $(OBJS) $(DB) $(TEST)

test: $(OBJS) $(TEST)
	$(CC) $(CFLAGS) -o $@ $^ $(CLINKS)

