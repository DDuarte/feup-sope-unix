CC=clang

CFLAGS= -std=gnu11 -g -Wall -pedantic -Wformat=2 -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Waggregate-return -Wmissing-prototypes -Werror -pedantic-errors
LDFLAGS= -lrt -lpthread -lm

SRC_DIR= src
BIN_DIR= bin
TEMP_DIR= temp

SRC_FILES_PATH= $(wildcard $(SRC_DIR)/*.c)
SRC_FILES= $(notdir $(SRC_FILES_PATH))
SRC_OBJ= $(addprefix $(BIN_DIR)/$(TEMP_DIR)/, $(SRC_FILES:.c=.o))

EXECUTABLE= tpc
EXECUTABLE_SRC= $(addsuffix .c, $(EXECUTABLE))
EXECUTABLE_SRC_FILE_PATH= $(addprefix $(SRC_DIR)/, EXECUTABLE_SRC)
EXECUTABLE_OBJ= $(addprefix $(BIN_DIR)/$(TEMP_DIR)/, $(EXECUTABLE_SRC:.c=.o))

.PHONY: all

all: dirs $(EXECUTABLE)

dirs:
	mkdir -p $(BIN_DIR) $(BIN_DIR)/$(TEMP_DIR)

$(EXECUTABLE): $(SRC_OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(SRC_OBJ) -o $(BIN_DIR)/$(basename $@)

%.o:
	$(CC) $(CFLAGS) -c $(SRC_DIR)/$(notdir $(basename $@)).c -o $(BIN_DIR)/$(TEMP_DIR)/$(notdir $(basename $@)).o

clean:
	rm -rf $(BIN_DIR)
