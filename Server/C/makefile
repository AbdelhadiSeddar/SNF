PROJECT = SRBNP
lib_name = srbnp

CC := c11
COMPILER := clang --std=$(CC)

C_FLAGS := 
C_LIB := -lm -luuid -lpthread

CURRENT_PATH = $(shell pwd)/

OBJ_PATH := $(CURRENT_PATH)obj/
LIB_PATH := $(CURRENT_PATH)lib/
SRC_PATH := $(CURRENT_PATH)src/
INC_PATH := $(CURRENT_PATH)include/

C_FILES := $(wildcard $(SRC_PATH)*.c)
H_FILES := $(wildcard $(SRC_PATH)*.h)
O_FILES := $(patsubst %.c,%.o,$(C_FILES))

.SILENT:

prebuild: clean 
	cp $(H_FILES) $(INC_PATH)$(PROJECT)

%.o: %.c
	gcc -o $@ -c $< -I$(INC_PATH) 

objbuild: prebuild $(O_FILES)  
	mv $(O_FILES) $(OBJ_PATH)
	
build: objbuild
	ar cr $(LIB_PATH)lib$(lib_name).a $(OBJ_PATH)*.o
	echo "Build: $(PROJECT) Success!\n result at $(LIB_PATH)lib$(lib_name).a call with -l$(lib_name)"

clean:
	rm -rf $(OBJ_PATH) $(INC_PATH)$(PROJECT) $(INC_PATH) $(LIB_PATH)
	mkdir $(OBJ_PATH) $(INC_PATH) $(INC_PATH)$(PROJECT) $(LIB_PATH)

all: build

test: build
	clang main.c -L$(LIB_PATH) -l$(lib_name) -I$(INC_PATH) $(C_FLAGS) $(C_LIB) -no-pie
	./a.out
