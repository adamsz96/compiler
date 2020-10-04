all: codeb

scanner.c: oxout.l
	flex -oscanner.c oxout.l 

scanner.o: scanner.c parser.h symbol_manager.h tree.h
	gcc -g -c -ansi -pedantic -Wall scanner.c -D_GNU_SOURCE

codeb: parser.o symbol_manager.o symbol_list.o code.o codegen.o tree.o scanner.o
	gcc -o codeb symbol_manager.o symbol_list.o parser.o codegen.o tree.o code.o scanner.o -lfl

list.o: list.c list.h
	gcc -c -ansi -pedantic -Wall list.c -D_GNU_SOURCE

tree.o: tree.c tree.h
	gcc -g -c -ansi -pedantic -Wall tree.c -D_GNU_SOURCE

test: test.o symbol_list.o symbol_manager.o
	gcc -o test test.o symbol_list.o symbol_manager.o

test.o: test.c
	gcc -g -c -ansi -pedantic -Wall test.c -D_GNU_SOURCE

symbol_list.o: symbol_list.c symbol_list.h
	gcc -g -c -ansi -pedantic -Wall symbol_list.c -D_GNU_SOURCE

symbol_manager.o: symbol_manager.c symbol_manager.h symbol_list.h
	gcc -g -c -ansi -pedantic -Wall symbol_manager.c -D_GNU_SOURCE

codegen.o: codegen.c codegen.h
	gcc -g -c -ansi -pedantic -Wall codegen.c

parser.o: parser.c list.h
	gcc -c -ansi -pedantic parser.c

parser.c parser.h: oxout.y
	yacc -d oxout.y -o parser.c

oxout.y oxout.l: parser.y scanner.l
	ox parser.y scanner.l

code.o: code.c tree.h
	gcc -g -c -ansi -pedantic -Wall code.c -D_GNU_SOURCE

code.c: code.bfe
	bfe < code.bfe | iburg > code.c

clean:
	rm -f ag *.o oxout.* lex.yy.c parser.o parser.h parser.c y.tab.h test test.o symbol_manager.o symbol_list.o
