CC = gcc -g
LEX = win_flex
YACC = win_bison
CFLAGS = -DYYDEBUG=1

PROGRAM = sql

all:	${PROGRAM}

sql:	sql.tab.o sql.o
	${CC} -o $@ sql.tab.o sql.o

sql.o:	sql.c
	${CC} ${CFLAGS} -c -o $@ $<

sql.c:	sql.l
	${LEX} -o $@ $<

sql.tab.o:	sql.tab.c
	${CC} ${CFLAGS} -c -o $@ $<

sql.tab.c sql.tab.h:	sql.y
	${YACC} -vd $<

clean:
	del sql.exe sql.tab.c sql.tab.h \
	sql.c sql.tab.o sql.o sql.output

.SUFFIXES:	.l .y .c