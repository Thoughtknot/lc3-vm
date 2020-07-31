make:
	gcc -Wall src/instructions.c -c -o src/instructions.o
	gcc -Wall src/memory.c -c -o src/memory.o
	gcc -Wall src/registers.c -c -o src/registers.o
	gcc -Wall src/util.c -c -o src/util.o
	gcc -Wall src/lc3vm.c -c -o src/lc3vm.o
	gcc -Wall -o lc3 src/*.o

check:
	gcc test/check_lc3.c -c -o test/check_lc3.o
	gcc src/instructions.o src/memory.o src/registers.o src/util.o test/*.o -o test_suite `pkg-config --cflags --libs check`
	./test_suite

clean:
	rm -r test/*.o
	rm -r src/*.o
	rm -r lc3
	rm -r test_suite

