CC = gcc -g
LEX = win_flex
YACC = win_bison
CFLAGS = -DYYDEBUG=1

PROGRAM = lpmysql

all:	${PROGRAM}

lpmysql:	lpmysql.tab.o lpmysql.o
	${CC} -o $@ lpmysql.tab.o lpmysql.o

lpmysql.o:	lpmysql.c
	${CC} ${CFLAGS} -c -o $@ $<

lpmysql.c:	lpmysql.l
	${LEX} -o $@ $<

lpmysql.tab.o:	lpmysql.tab.c
	${CC} ${CFLAGS} -c -o $@ $<

lpmysql.tab.c lpmysql.tab.h:	lpmysql.y
	${YACC} -vd $<

clean:
	del lpmysql.exe lpmysql.tab.c lpmysql.tab.h \
	lpmysql.c lpmysql.tab.o lpmysql.o lpmysql.output

.SUFFIXES:	.l .y .c