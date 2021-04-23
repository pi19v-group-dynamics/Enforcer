CC = gcc
NAME = poke-n-conquer 
FLAGS = -Wall -Wextra -pedantic -march=native -O0 -g03 \
				-Ilib/ -Isrc/\
				-DLOG_ALLOW=LOG_ALL -include src/log.h \
				-DSYS_INIT_WINDOW_SCALE=4 \
				-DSYS_INIT_WINDOW_TITLE="\"Poke n' Conquer\"" \
				-DMEMALLOC_ABORT -DMEMALLOC_LOG

ifeq ($(OS), Windows_NT)
	FLAGS += -lmingw32 -lSDL2main -lSDL2
else
	FLAGS += -lSDL2 -lm
endif

build:
ifeq ($(OS), Windows_NT)
	if not exist bin mkdir bin
else
	mkdir -p bin
endif
	$(CC) $(wildcard src/*.c src/stages/*.c) -obin/$(NAME) $(FLAGS)

run:
	./bin/$(NAME)

debug:
	gdb bin/$(NAME)

clean:
ifeq ($(OS), Windows_NT)
	del bin\$(NAME).exe
else
	rm -f bin/$(NAME)
endif

