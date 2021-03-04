.PHONY: CC NAME FLAGS

CC = gcc
NAME = tetrix
FLAGS = -Wall -Wextra -pedantic -g03 -O0 \
				-DLOG_ALLOW=LOG_ALL -include src/log.h \
				-DSYS_INIT_WINDOW_SCALE=4 \
				-DSYS_INIT_WINDOW_TITLE="\"Untitled game\"" \
				-DMEMALLOC_ABORT
				# -DMEMALLOC_LOG

#ifeq ($(OS), Windows_NT)
#	FLAGS += -lmingw32 -lSDL2main -lSDL2
#else
#ifeq ($(UNAME_S), Linux)
FLAGS += -lSDL2 -lm
#else
#	$(error Your platform not supported)
#endif
#endif

build:
ifeq ($(OS), Windows_NT)
	if not exist bin mkdir bin
else
	mkdir -p bin
endif
	$(CC) $(wildcard src/*.c) -obin/$(NAME) $(FLAGS)

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

