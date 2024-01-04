COMPILER=clang
flags=-lpthread -luuid -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -DTHREADSAFE=1

SQLite_PATH=SQLite/sqlite3.o

BIN_PATH=/var/tmp/SRBNP
BUILD_PATH=bin
all: deploy

deployprepare:

deploy:
	make clean;
	if [ ! -d "$(BIN_PATH)" ]; then mkdir $(BIN_PATH); fi;
	make main;
	cp $(BUILD_PATH)/* $(BIN_PATH)
	bash _service_deploy.sh
	
clean:
	if [ -f "$(BIN_PATH)/SRBNP.run" ]; then rm $(BIN_PATH)/SRBNP.run; fi;
main:
	if [ ! -d "$(BUILD_PATH)" ]; then mkdir $(BUILD_PATH); fi;
	if [ ! -f "$(SQLite_PATH)" ]; then cd SQLite; make build; fi;
	$(COMPILER) main.c SRBNP/*.c SQLite/*.o -o $(BUILD_PATH)/SRBNP.run $(flags)

adrsan:
	if [ ! -d "$(BUILD_PATH)" ]; then mkdir $(BUILD_PATH); fi;
	if [ ! -f "$(SQLite_PATH)" ]; then cd SQLite; make build; fi;
	$(COMPILER) main.c SRBNP/*.c SQLite/*.o -o $(BUILD_PATH)/SRBNP.run $(flags) -g -fsanitize=address



	
