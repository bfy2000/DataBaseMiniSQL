CC = gcc -g
LEX = win_flex
YACC = win_bison
CFLAGS = -DYYDEBUG=1

PROGRAM = pmysql

all:	${PROGRAM}

pmysql:	pmysql.tab.o pmysql.o
	${CC} -o $@ pmysql.tab.o pmysql.o

pmysql.o:	pmysql.c
	${CC} ${CFLAGS} -c -o $@ $<

pmysql.c:	pmysql.l
	${LEX} -o $@ $<

pmysql.tab.o:	pmysql.tab.c
	${CC} ${CFLAGS} -c -o $@ $<

pmysql.tab.c pmysql.tab.h:	pmysql.y
	${YACC} -vd $<

clean:
	del pmysql.exe pmysql.tab.c pmysql.tab.h \
	pmysql.c pmysql.tab.o pmysql.o pmysql.output

.SUFFIXES:	.l .y .c