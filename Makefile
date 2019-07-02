
include version.mak

ROOT_DIR=$(shell pwd)
SRC_DIR:=$(ROOT_DIR)/src
INC_DIR:=$(ROOT_DIR)/include
TEST_DIR:=$(ROOT_DIR)/test

CC:=gcc
AR:=ar

CFLAGS:=-std=c89 -O2 -Wall -Werror -fPIC -DVERSION="\$(VER_DATE)\" -I$(INC_DIR)
LDFLAGS:=
LIBS:=

export CC AR CFLAGS LDFLAGS LIBS VER

default:
	make -C $(SRC_DIR) -f src.mak
	ar crs libminids-$(VER).a $(SRC_DIR)/*.o
	make -C $(TEST_DIR) -f test.mak

clean:
	make -C $(SRC_DIR) clean -f src.mak
	make -C $(TEST_DIR) clean -f test.mak
	rm -f *.a

.PHONY:default clean libs
