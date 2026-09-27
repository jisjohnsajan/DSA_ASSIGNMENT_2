# Makefile - Hash Table using the Division Method
# DSA Assignment, Question 1
#
#   make          build the program
#   make run      build and run, saving the output to output/program_output.txt
#   make clean    remove build artefacts

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O2
TARGET  = build/hash_table
SRC     = src/hash_table.c
DATA    = input/song_ids.txt
OUT     = output/program_output.txt

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	mkdir -p output
	./$(TARGET) $(DATA) | tee $(OUT)
	@echo ""
	@echo "Output written to $(OUT)"

clean:
	rm -rf build
