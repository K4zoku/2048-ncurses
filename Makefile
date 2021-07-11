CC :=gcc
CCFLAGS :=-Wall -Wextra -Wnonnull -O2
CC_LIBRARIES :=-lncursesw

INSTALL_DIR:=bin
SRC:=src
INCLUDES:=src
TARGET:=out
OBJ:=obj
BIN:=$(TARGET)/bin

CWD:=$(shell echo %cd%)
SOURCES:=$(shell dir /b/d/s/a-d $(SRC)\*.c)
SOURCES:=$(subst $(CWD)\,,$(SOURCES))
SOURCES:=$(subst \,/,$(SOURCES))
OBJECTS:=$(addprefix $(TARGET)/$(OBJ), $(subst $(SRC),,$(SOURCES:c=o)))

all: clean package

clean:
	@echo Cleaning...
	@rm -rf $(TARGET)
	@rm -f $(INSTALL_DIR)\2048.exe
	@rm -f $(INSTALL_DIR)\2048.zip

prepare:
	@echo Preparing...
	@for %%d in ($(subst $(SRC),$(TARGET)/$(OBJ),$(shell dir /b/d/s/ad $(SRC)))) \
		do if not exist %%d ( md "%%d" )
	@if not exist $(BIN) ( md "$(BIN)" )

$(TARGET)/$(OBJ)/%.o: $(SRC)/%.c
	@echo Compiling $<
	@$(CC) -I$(INCLUDES) $(CCFLAGS) -c $< -o $@ $(CC_LIBRARIES)

compile: $(OBJECTS)
	@$(CC) -I$(INCLUDES) $(CCFLAGS) $^ -o $(BIN)\2048.exe $(CC_LIBRARIES)

package: prepare compile
	@echo Packaging...
	@if not exist $(INSTALL_DIR) ( md $(INSTALL_DIR) )
	@cp -f $(BIN)\2048.exe $(INSTALL_DIR)\2048.exe
	@cp -u lib\libncursesw6.dll $(INSTALL_DIR)\libncursesw6.dll
	@zip -q -9 bin\2048.zip bin\2048.exe lib\libncursesw6.dll

run: package
	@echo Running...
	$(INSTALL_DIR)\2048.exe
