CSTD = gnu99
SRC = utils.c luts.c encoding.c ecc.c qrcode_gen.c vis.c qreator.c
OBJ = ${SRC:.c=.o}
CFLAGS = -std=${CSTD}

all: qreator

debug: CFLAGS += -Wall -Wextra -g -pedantic
debug: LFLAGS += -fsanitize=address
debug: qreator

gdb: CFLAGS += -Wall -Wextra -g -pedantic
gdb: qreator

release: CFLAGS += -O3
release: qreator

qreator: ${OBJ}
	${CC} ${OBJ} ${LIBS} ${LFLAGS} -o $@

clean: qreator
	rm -f ${OBJ}
