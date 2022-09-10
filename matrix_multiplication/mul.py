import numpy as np
from time import perf_counter

SIZE_N = 1024
# SIZE_N = 1024*8 

print("The size of the matrix is:", SIZE_N, "x",SIZE_N)

def with_numpy():
    A = np.random.randn(SIZE_N, SIZE_N).astype(np.float32)
    B = np.random.randn(SIZE_N, SIZE_N).astype(np.float32)
    start = perf_counter()
    C = A @ B
    end = perf_counter()
    s = end-start
    flop = 2*SIZE_N*SIZE_N*SIZE_N
    print(f"{flop/s * 1e-9:.2f} GFLOP/s")


if __name__ == "__main__":
    with_numpy()
