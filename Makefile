
include version.mak

ROOT_DIR=$(shell pwd)
SRC_DIR:=$(ROOT_DIR)/src
TEST_DIR:=$(ROOT_DIR)/test

CC:=gcc

CFLAGS:=-std=c89 -O2 -Wall -Werror -fPIC -DVERSION="\$(VER_DATE)\"
LDFLAGS:=
LIBS:=

export CC AR CFLAGS LDFLAGS LIBS VER

all:
	make -C $(SRC_DIR) -f src.mak
	make -C $(TEST_DIR) -f test.mak

clean:
	make -C $(SRC_DIR) clean -f src.mak
	make -C $(TEST_DIR) clean -f test.mak

.PHONY:all clean libs
