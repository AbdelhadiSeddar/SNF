C=c11
COMPILER=clang --std=$C
flags=-lpthread -luuid -lm


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
	$(COMPILER) main.c SRBNP/*.c -o $(BUILD_PATH)/SRBNP.run $(flags)

adrsan:
	if [ ! -d "$(BUILD_PATH)" ]; then mkdir $(BUILD_PATH); fi;
	$(COMPILER) main.c SRBNP/*.c -o $(BUILD_PATH)/SRBNP.run $(flags) -g -fsanitize=address
thrdsan:
	if [ ! -d "$(BUILD_PATH)" ]; then mkdir $(BUILD_PATH); fi;
	$(COMPILER) main.c SRBNP/*.c -o $(BUILD_PATH)/SRBNP.run $(flags) -g -fsanitize=thread


