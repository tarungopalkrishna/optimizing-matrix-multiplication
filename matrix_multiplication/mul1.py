import os
# os.environ['OMP_NUM_THREADS'] = '1'
import time
import numpy as np

N = 2048
if __name__ == "__main__":
  # N^2
  A = np.random.randn(N, N).astype(np.float32)
  # N^2
  B = np.random.randn(N, N).astype(np.float32)

  for i in range(4):
    st = time.monotonic()
    C = A @ B.T
    et = time.monotonic()
    s = et-st
    # 2N compute in N^2 output cells
    flop = 2*N*N*N
    #print(f"{flop / 1e9:.2f} GFLOP")
    print(f"{flop/s * 1e-9:.2f} GFLOP/S, {s*1e3:.2f} ms")

  with open("./matmul_matrix", "wb") as f:
    f.write(A.data)
    f.write(B.data)
    f.write(C.data)

  print(A[0][0], B[0][0], C[0][0])
