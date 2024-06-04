PROJECT = SNF
lib_name = snf

STD_LIB_PATH = $(shell cat /etc/ld.so.conf.d/libc.conf | grep /)
STD_INC_PATH = /usr/include

CC := c11
COMPILER := clang --std=$(CC)

C_FLAGS := -g
C_LIB := -lm -luuid -lpthread

CURRENT_PATH = $(shell pwd)/

OBJ_PATH := $(CURRENT_PATH)obj/
LIB_PATH := $(CURRENT_PATH)lib/
SRC_PATH := $(CURRENT_PATH)src/
INC_PATH := $(CURRENT_PATH)include/
TST_PATH := $(CURRENT_PATH)tests/

C_FILES := $(wildcard $(SRC_PATH)*.c)
H_FILES := $(wildcard $(SRC_PATH)*.h)
O_FILES := $(patsubst %.c,%.o,$(C_FILES))

.SILENT:

all: build

%.h:
	echo "#include <$(PROJECT)/$@>" >> $(INC_PATH)$(PROJECT).h

headers: heads
	echo "#endif" >> $(INC_PATH)$(PROJECT).h

heads: clean prepare $(patsubst $(SRC_PATH)%.h,%.h,$(H_FILES))
	echo "##	Copying Headers"
	cp $(H_FILES) $(INC_PATH)$(PROJECT)

%.o: %.c
	$(COMPILER) -o $@ -c $< -I$(INC_PATH) $(C_FLAGS)

objbuild: headers $(O_FILES)
	echo "##	Building Objects"
	mv $(O_FILES) $(OBJ_PATH)

build: objbuild
	echo "##	Building Library"
	ar cr $(LIB_PATH)lib$(lib_name).a $(OBJ_PATH)*.o
	echo "##	Successfully Built $(PROJECT) ( lib$(lib_name).a )"

clean:
	echo "##	Cleaning paths"
	rm -rf $(OBJ_PATH) $(INC_PATH)$(PROJECT) $(INC_PATH) $(LIB_PATH)
	rm -f $(TST_PATH)*.out $(SRC_PATH)*.o

cleanlib:
	echo "##	Cleaning standard library and include paths from $(PROJECT)"
	echo "##	Asking for permission. "
	sudo rm -rf $(STD_INC_PATH)/$(PROJECT)
	sudo rm -f $(STD_INC_PATH)/$(PROJECT).h $(STD_LIB_PATH)/lib$(lib_name).a

prepare:
	echo "##	Preparing Paths"
	mkdir $(OBJ_PATH) $(INC_PATH) $(INC_PATH)$(PROJECT) $(LIB_PATH)
	touch $(INC_PATH)$(PROJECT).h
	echo "##	Preparing $(PROJECT).h"
	cat $(PROJECT).h > $(INC_PATH)$(PROJECT).h

install: build cleanlib
	sudo rm -rf $(STD_INC_PATH)/$(PROJECT)
	sudo mkdir $(STD_INC_PATH)/$(PROJECT)
	echo "##	Installing headerfiles"
	sudo mv $(INC_PATH)$(PROJECT)/*.h $(STD_INC_PATH)/$(PROJECT)
	sudo mv $(INC_PATH)$(PROJECT).h $(STD_INC_PATH)
	echo "##	Haderfiles installed at $(STD_INC_PATH)/ :"
	ls -l $(STD_INC_PATH)/$(PROJECT)
	echo "##	Installing library: lib$(lib_name).a"
	sudo mv $(LIB_PATH)lib$(lib_name).a $(STD_LIB_PATH)/
	echo "##	Library installed at $(STD_LIB_PATH)/"
	echo "##	Done! link library with -l$(lib_name)"
	echo "##		Note: must also link [ $(C_LIB) ]"

buildtest: build
	echo "##	Building test main"
	$(COMPILER) main.c -L$(LIB_PATH) -l$(lib_name) -I$(INC_PATH) $(C_FLAGS) $(C_LIB) -no-pie

test: buildtest
	./a.out

testgdb: buildtest
	gdb a.out

testinstall : install
	$(COMPILER) main.c -l$(lib_name) $(C_FLAGS) $(C_LIB) -no-pie
	./a.out

t_%: install
	$(COMPILER) $(TST_PATH)$(patsubst t_%,%, $@).c \
	-l$(lib_name)  $(C_FLAGS) $(C_LIB) -no-pie \
	-o $(TST_PATH)$@.out
	$(TST_PATH)$@.out
gdbt_%: install
	$(COMPILER) $(TST_PATH)$(patsubst gdbt_%,%, $@).c \
	-l$(lib_name)  $(C_FLAGS) $(C_LIB) -no-pie \
	-o $(TST_PATH)$@.out
	gdb $(TST_PATH)$@.out