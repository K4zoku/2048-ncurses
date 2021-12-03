CC:=gcc
CCFLAGS:=-Wall -Wextra -Wnonnull
CCLIBS:=-lm -lncursesw -ltinfo
CCEXTRAS:=-fPIC -O2 -g
SRC:=src
INCLUDES:=src
TARGET:=out
OBJ:=obj
BIN:=bin

SOURCES:=$(shell find $(SRC) -name '*.c')
OBJECTS:=$(SOURCES:$(SRC)/%.c=$(OBJ)/%.o)

L_TARGET:=$(TARGET)/linux
L_BIN:=$(L_TARGET)/$(BIN)
L_OBJECTS:=$(addprefix $(L_TARGET)/, $(OBJECTS))

MAKELISTS:=$(patsubst %.c,%,$(subst $(PWD)/$(SRC)/,,$(SOURCES)))

default: clean compile

clean:
	rm -rf $(L_BIN)/*
	rm -rf $(L_OBJECTS)/*

compile: $(L_OBJECTS)
	mkdir -p $(L_BIN)
	$(CC) -I$(INCLUDES) $(CCFLAGS) $^ -o $(L_BIN)/2048 $(CCLIBS) $(CCEXTRAS)

$(TARGET)/linux/$(OBJ)/%.o: $(SRC)/%.c
	mkdir -p $$(dirname $@)
	$(CC) -I$(INCLUDES) $(CCFLAGS) -c $< -o $@ $(CCLIBS) $(CCEXTRAS)

install: compile
	mkdir -p /opt/2048
	cp -uf $(L_BIN)/2048 /opt/2048/
	ln -sf /opt/2048/2048 /usr/bin/

uninstall: 
	rm -rf /opt/2048
	rm -f /usr/bin/2048
