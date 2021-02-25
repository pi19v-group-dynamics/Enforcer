NAME = tetrix
CC = gcc
WARN = -Wall -Wextra -pedantic
INC = -include src/common.h
MEMALLOC = -DMEMALLOC_ABORT # -DMEMALLOC_LOG
LOG = -DLOG_ALLOW=LOG_ALL
FLAGS = $(WARN) $(INC) $(LIB) $(MEMALLOC) $(LOG)
TEST = -g -DDEBUG -O0
RELEASE = -fPIC -Of

ifeq ($(OS), Windows_NT)
	REL += -mwindows
	LIB = -lmingw32 -lSDL2main
else
	ifeq ($(UNAME_S), Linux)
		LIB = -lm
	else
		$(error Your platform not supported)
	endif
endif

LIB += -lSDL2

FLAGS += -DP_TARGET_FPS=30.0

all: build

build:
ifeq ($(OS), Windows_NT)
	if not exist bin mkdir bin
else
	mkdir -p bin
endif
	$(CC) src/*.c -obin/$(NAME) $(FLAGS) $(TEST)

release:
ifeq ($(OS), Windows_NT)
	if not exist bin mkdir bin
else
	mkdir -p bin
endif
	$(CC) src/*.c -obin/$(NAME) $(FLAGS) $(REL)

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

