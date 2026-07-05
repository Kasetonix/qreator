CSTD = gnu99
SRC = utils.c data_structs.c  encoding.c qrcode_gen.c vis.c qreator.c
OBJ = ${SRC:.c=.o}
# LIBS = -lcurses
CFLAGS = -std=${CSTD}

all: qreator

debug: CFLAGS += -Wall -Wextra -Werror -g -pedantic
debug: LFLAGS += -fsanitize=address
debug: qreator

release: CFLAGS += -O3
release: qreator

qreator: ${OBJ}
	${CC} ${OBJ} ${LIBS} ${LFLAGS} -o $@

clean: qreator 
	rm -f ${OBJ}
