PROJECT = SRBNP
lib_name = srbnp

STD_LIB_PATH = $(shell cat /etc/ld.so.conf.d/libc.conf | grep /)
STD_INC_PATH = /usr/include

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


%.h:
	echo "include <$(PROJECT)/$@>" >> $(INC_PATH)$(PROJECT).h

headers: clean prepare $()
	echo $(H_FILES)
	cp $(H_FILES) $(INC_PATH)$(PROJECT)

%.o: %.c
	$(COMPILER) -o $@ -c $< -I$(INC_PATH) 

objbuild: headers $(O_FILES)  
	mv $(O_FILES) $(OBJ_PATH)
	
build: objbuild
	ar cr $(LIB_PATH)lib$(lib_name).a $(OBJ_PATH)*.o
	echo "Build: $(PROJECT) Success!\n result at $(LIB_PATH)lib$(lib_name).a call with -l$(lib_name)"

clean:
	rm -rf $(OBJ_PATH) $(INC_PATH)$(PROJECT) $(INC_PATH) $(LIB_PATH)
	rm -f $(INC_PATH)$(PROJECT).h

prepare:
	mkdir $(OBJ_PATH) $(INC_PATH) $(INC_PATH)$(PROJECT) $(LIB_PATH)
	touch $(INC_PATH)$(PROJECT).h
	echo "// Main Source file" > $(INC_PATH)$(PROJECT).h

all: build

install: build
	echo "##	Asking for permission. "
	sudo rm -rf $(STD_INC_PATH)/$(PROJECT)
	sudo mkdir $(STD_INC_PATH)/$(PROJECT)
	echo "##	Installing headerfiles"
	sudo mv $(INC_PATH)$(PROJECT)/*.h $(STD_INC_PATH)/$(PROJECT) 
	echo "##	Haderfiles installed at $(STD_INC_PATH)/"
	ls -l $(STD_INC_PATH)/$(PROJECT) 
	echo "##	Installing library: lib$(lib_name).a"
	sudo mv $(LIB_PATH)lib$(lib_name).a $(STD_LIB_PATH)/
	echo "##	Library installed"
	echo "##	Done! link library with -l$(lib_name)"
	echo "##		Note: must also link [ $(C_LIB) ]"

test: build
	clang main.c -L$(LIB_PATH) -l$(lib_name) -I$(INC_PATH) $(C_FLAGS) $(C_LIB) -no-pie
	./a.out

testinstall : install
	clang main.c -l$(lib_name) $(C_FLAGS) $(C_LIB) -no-pie
	./a.out