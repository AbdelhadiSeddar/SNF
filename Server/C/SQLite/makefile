COMPILER=/bin/gcc
SQLite=sqlite3

all: build

build:
	if [ ! -f "$(SQLite).o" ]; then $(COMPILER) -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -lpthread -ldl -o $(SQLite).o -c $(SQLite).c ; fi;
clean:
	if [ -f "$(SQLite).o" ]; then /bin/rm $(SQLite).o fi;