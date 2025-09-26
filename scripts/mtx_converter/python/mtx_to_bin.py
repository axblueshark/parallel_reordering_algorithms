from petsc4py import PETSc
from mpi4py import MPI
import sys
import numpy as np
from scipy.io import mmread
import os


def is_symmetric_from_filename(filename):
    return "_s_" in os.path.basename(filename)


def main():
    if len(sys.argv) != 3:
        print("Expected: python3 mtx_to_bin_converter.py input.mtx output.bin")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    is_symmetric = is_symmetric_from_filename(input_file)

    A_local = mmread(input_file).tocsr()
    A_local = A_local.astype(np.float64)

    # get shape and local row distribution
    M, N = A_local.shape
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    rows_per_proc = M // size
    remainder = M % size
    if rank < remainder:
        rstart = rank * (rows_per_proc + 1)
        rend = rstart + rows_per_proc + 1
    else:
        rstart = rank * rows_per_proc + remainder
        rend = rstart + rows_per_proc

    # prepare petsc matrix
    if is_symmetric:
        mat_type = PETSc.Mat.Type.MPISBAIJ  # parallel symmetric
    else:
        mat_type = PETSc.Mat.Type.MPIAIJ    # parallel unsymmetric

    A = PETSc.Mat().create(comm=comm)
    A.setSizes([[rend - rstart, M], [N, N]])
    A.setType(mat_type)
    A.setFromOptions()
    A.setUp()

    for i_local in range(rend - rstart):
        i_global = rstart + i_local
        row = A_local.getrow(i_global)
        cols = row.indices
        vals = row.data

        for j, val in zip(cols, vals):
            A.setValue(i_global, j, val, addv=False)
            if is_symmetric and i_global != j:
                A.setValue(j, i_global, val, addv=False)

    A.assemble()

    viewer = PETSc.Viewer().createBinary(output_file,
                                         mode=PETSc.Viewer.Mode.WRITE,
                                         comm=comm)
    A.view(viewer)
    viewer.destroy()
    A.destroy()

    if rank == 0:
        print(f"Converted: {input_file} → {output_file}")


if __name__ == "__main__":
    main()
