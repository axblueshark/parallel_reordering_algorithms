# Parallel reordering algorithms and parallel direct solvers

This repository is an integral part of the bachelor thesis **Parallel reordering algorithms and parallel direct solvers**. It provides an environment for running parallel numerical experiments, demonstrating effect of different reordering algorithms (e.g. AMD, ND, METIS, ...) along with different direct solvers, such as MUMPS, SuperLU_DIST or PaStiX, to mention a few, on computational time, fill-in, memory requirements and other relevant metrics.

The results will be presented in the **Numerical experiments** chapter of the aforementioned thesis.

## Repository overview and structure
```
numerical_experiments/
│
├── matrices/                 
│   ├── mtx/  # here the matrices in .mtx format should be placed
│   ├── bin/  # here the converted matrix will be stored
│   └── matrices.md
│
├── plots/
│   ├── sparsity_structure/ 
│   └── plots.ipynb
│
├── results/
│   ├── mumps/
│   ├── pastix/
│   ├── superlu_dist/
│   └── results.ipynb  # batch run experiments and obtain results
│
│── scripts/
│   ├── mtx_converter/ # converter from .mtx .bin
│   ├── experiments.c  # the main experiments driver
│   ├── loading.c      # loading (or generating) data
│   ├── Makefile
│   ├── profiling.c    # obtaining basic statistics
│   ├── reorder.c      # performing reordering
│   ├── solve.c        # solving the system 
│   └── solver_utils.h # all available functions headers
│── .gitignore
└── README.md
```

## Requirements
- [**PETSc**](https://petsc.org/release/)
  built with:
  - MPI support
  - BLAS / LAPACK
  - all relevant direct solvers (MUMPS, SuperLU_DIST, PaStiX)
  - all relevant reorderings (for other related dependencies, see PETSc documentation)

- **MPI implementation**  
  - [OpenMPI](https://www.open-mpi.org/) or [MPICH](https://www.mpich.org/)  

- **C compiler**  
  - GCC ≥ 9, Clang ≥ 12, or Intel oneAPI  

- **Python 3 + Jupyter + Matplotlib**: for batch running (see results.ipynb) analysis/plotting 


## Installation

Clone the repository:
```bash
git clone https://github.com/axbluefish/parallel_reordering_algorithms.git
cd numerical_experiments
```

Then build with:
```bash
cd scripts
make
```

This will produce the `experiment` file, which is the main executable for the project.

## Running

After successfully building the project, follow these steps to obtain results: 

1. **Download test matrices** \
Place `.mtx` files into `matrices/mtx/`.

2. **Convert to PETSc binary**
Preferably run the scripts in `scripts/mtx_converter/mtx_to_bin.ipynb`. This will convert the `.mtx` matrices into `.bin` format in `matrices/bin` subfolder.

3. **Run**
In `results/results.ipynb`, the scripts are ready to be used. Adjust solvers, reorderings and number of processes here.

Jupyter notebooks are intended for systematic batch runs and plotting, while command-line runs are useful for quick debugging. If you want to run results from command line, see the following example:

```bash
mpirun -n 4 ./experiment \
  -mat_file ../matrices/bin/matrix_name.bin \
  -pc_type lu \
  -mat_solver_type mumps \
  -mat_ordering_type amd \
  -own_reordering true \
  -get_solution_norm \
  -log_view
```

