CC=gcc

CFLAGS= -Wall -std=gnu11
LDFLAGS=

SRC_DIR= src
BIN_DIR= bin
TEMP_DIR= temp

EXECUTABLE_SRC= $(notdir $(wildcard $(SRC_DIR)/*.c))
SOURCES= $(notdir $(EXECUTABLE_SRC))
EXECUTABLE=$(notdir $(basename $(SOURCES)))
EXECUTABLE_OBJ=$(addprefix $(SRC_DIR)/, $(SOURCES:.c=.o))

LIB_DIR= lib
LIB_SRC_FILES= $(notdir $(LIB_SRC))
LIB_SRC= $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ= $(addprefix $(LIB_DIR)/, $(LIB_SRC_FILES:.c=.o))

OBJS = $(addprefix $(BIN_DIR)/$(TEMP_DIR)/, $(LIB_SRC_FILES:.c=.o))

.PHONY: all

all: dirs $(LIB_OBJ) $(EXECUTABLE_OBJ) $(EXECUTABLE)

dirs:
	mkdir -p $(BIN_DIR) $(BIN_DIR)/$(TEMP_DIR)

$(EXECUTABLE): $(LIB_OBJ) $(EXECUTABLE_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(BIN_DIR)/$(TEMP_DIR)/$@.o -o $(BIN_DIR)/$(basename $@)

%.o:
	$(CC) $(CFLAGS) -c $(basename $@).c -o $(BIN_DIR)/$(TEMP_DIR)/$(notdir $(basename $@)).o -I./$(LIB_DIR)

clean:
	rm -rf $(BIN_DIR)