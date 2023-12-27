COMPILER=/bin/clang
BIN_PATH=/var/tmp/SRBNP
BUILD_PATH=bin
all: deploy

deploy:
	/bin/make clean
	if [ ! -d "$(BIN_PATH)" ]; then /bin/mkdir $(BIN_PATH); fi
	/bin/cp $(BUILD_PATH)/* $(BIN_PATH)
	/bin/make main
	/bin/sh _service_deploy

clean:
	if [ -f "$(BIN_PATH)/SRBNP.run" ]; then /bin/rm $(BIN_PATH)/SRBNP.run; fi;
main:
	if [ ! -d "$(BUILD_PATH)" ]; then /bin/mkdir $(BUILD_PATH); fi;
	$(COMPILER) *.c */*.c -o $(BUILD_PATH)/SRBNP.run