all: jobshop

stack.o: stack.cc stack.h
	g++ stack.cc -ggdb -Wall -c -o stack.o

intervals.o: intervals.cc intervals.h
	g++ intervals.cc -ggdb -Wall -c -o intervals.o

operation.o: operation.cc operation.h
	g++ operation.cc -ggdb -Wall -c -o operation.o

main.o: main.cc operation.h
	g++ main.cc -ggdb -Wall -c -o main.o

jobshop: main.o operation.o intervals.o stack.o
	g++ main.o operation.o intervals.o stack.o -ggdb -o jobshop 

pure: main.o operation.o intervals.o stack.o
	purify g++ -ggdb -o jobshop main.o operation.o intervals.o stack.o

clean:
	rm *.o jobshop