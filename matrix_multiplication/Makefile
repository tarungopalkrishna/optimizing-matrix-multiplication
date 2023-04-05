# Dont use -pg and valgrind
# https://stackoverflow.com/questions/2146082/valgrind-profiling-timer-expired


all:
	gcc -o read_np_data -O2 read_np_data.c
	gcc -o basic_multiply basic_multiply.c
	gcc -o row_major_multiply row_major_multiply.c
	gcc -O2 -o row_major_multiply_o2 row_major_multiply.c
	gcc -O2 -march=native -o parallel_multiply_o2 parallel_multiply.c -lpthread
	gcc -O2 -march=native -o tiling_multiply_o2 tiling_multiply.c -lpthread
	gcc -O2 -march=native -o tiling_parallel_multiply_o2 tiling_parallel_multiply.c -lpthread
	gcc -O2 -march=native -o simd_parallel_multiply simd_parallel_multiply.c -lpthread

debug:
	gcc -DDEBUG -g -ggdb3 -o debug_basic_multiply basic_multiply.c
	gcc -DDEBUG -g -ggdb3 -o debug_row_major_multiply row_major_multiply.c
	gcc -DDEBUG -g -ggdb3 -O2 -o debug_row_major_multiply_o2 row_major_multiply.c
	gcc -DDEBUG -g -ggdb3 -march=native -O2 -o debug_parallel_multiply_o2 parallel_multiply.c -lpthread
	gcc -DDEBUG -g -ggdb3 -O2 -march=native -o debug_tiling_multiply_o2 tiling_multiply.c -lpthread
	gcc -DDEBUG -g -ggdb3 -O2 -march=native -o debug_tiling_parallel_multiply_o2 tiling_parallel_multiply.c -lpthread
	gcc -DDEBUG -g -ggdb3 -O2 -march=native -o debug_simd_parallel_multiply simd_parallel_multiply.c -lpthread

stats:
	valgrind --tool=cachegrind --log-file=basic_multiply.cachegrind.out ./basic_multiply
	valgrind --tool=cachegrind --log-file=row_major_multiply.cachegrind.out ./row_major_multiply
	valgrind --tool=cachegrind --log-file=row_major_multiply_o2.cachegrind.out ./row_major_multiply_o2
	valgrind --tool=cachegrind --log-file=parallel_multiply_o2.cachegrind.out ./parallel_multiply_o2

tabulate:
	rm -f a.csv
	echo "basic_multiply" >> a.csv
	for run in 1 2 3; do ./basic_multiply | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done
	echo "row_major_multiply" >> a.csv
	for run in 1 2 3; do ./row_major_multiply | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done
	echo "row_major_multiply_o2" >> a.csv
	for run in 1 2 3; do ./row_major_multiply_o2 | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done
	echo "parallel_multiply_o2" >> a.csv
	for run in 1 2 3; do ./parallel_multiply_o2 | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done
	echo "tiling_multiply_o2" >> a.csv
	for run in 1 2 3; do ./tiling_multiply_o2 | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done
	echo "tiling_parallel_multiply_o2" >> a.csv
	for run in 1 2 3; do ./tiling_parallel_multiply_o2 | grep -E 'GFLOPS/S|start|size' | tr '\n' ',' | awk '{print $$"\n"}' >> a.csv; done


clean:
	rm -f a.csv
	rm -f basic_multiply
	rm -f row_major_multiply
	rm -f row_major_multiply_o2
	rm -f parallel_multiply_o2
	rm -f tiling_multiply_o2
	rm -f tiling_parallel_multiply_o2
	rm -f debug_*
	rm -f *.cachegrind.out
	rm -f cachegrind.out.*
	rm -f matmul_matrix
	rm -f read_np_data
	rm -f simd_parallel_multiply
