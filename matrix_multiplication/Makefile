# Dont use -pg and valgrind
# https://stackoverflow.com/questions/2146082/valgrind-profiling-timer-expired


all:
	gcc -o basic_multiply basic_multiply.c
	gcc -o row_major_multiply row_major_multiply.c
	gcc -O2 -o row_major_multiply_o2 row_major_multiply.c
	# gcc -O3 -o basic_multiply basic_multiply.c
	# gcc -O3 -o basic_multiply basic_multiply.c

debug:
	gcc -DDEBUG -g -ggdb3 -o debug_basic_multiply basic_multiply.c
	gcc -DDEBUG -g -ggdb3 -o debug_row_major_multiply row_major_multiply.c
	gcc -DDEBUG -g -ggdb3 -O2 -o debug_row_major_multiply_o2 row_major_multiply.c

stats:
	valgrind --tool=cachegrind --log-file=basic_multiply.cachegrind.out ./basic_multiply
	valgrind --tool=cachegrind --log-file=row_major_multiply.cachegrind.out ./row_major_multiply
	valgrind --tool=cachegrind --log-file=row_major_multiply_o2.cachegrind.out ./row_major_multiply_o2
