build:
	g++ -g Test.cpp

run: build
	./a.out

valgrind: build
	valgrind --tool=memcheck ./a.out
