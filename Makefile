CSTD = gnu99
SRC = utils.c luts.c encoding.c ecc.c qrcode_gen.c vis.c qreator.c
OBJ = ${SRC:.c=.o}
CFLAGS = -std=${CSTD}

all: qreator

light_asan: CFLAGS += -DLIGHT_MODE 
light_asan: asan 

light_debug: CFLAGS += -DLIGHT_MODE 
light_debug: debug 

light: CFLAGS += -DLIGHT_MODE 
light: release

asan: LFLAGS += -fsanitize=address
asan: debug 

debug: CFLAGS += -Wall -Wextra -g -pedantic
debug: qreator

release: CFLAGS += -O3
release: qreator

qreator: ${OBJ}
	${CC} ${OBJ} ${LIBS} ${LFLAGS} -o $@

clean: qreator
	rm -f ${OBJ}
