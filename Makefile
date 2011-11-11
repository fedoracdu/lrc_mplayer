TARGET = lrc
OBJS = main.o bluray.o common.o cue.o gbk.o lrc.o music_play.o wildcard.o
CFLAGS = -c -Wall -I./include

All: ${OBJS}
	gcc -Wall ${OBJS} -o ${TARGET}

.c.o:
	gcc ${CFLAGS} $<
clean:
	rm ${TARGET} ${OBJS}
