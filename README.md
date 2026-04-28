# Parallel reordering algorithms and parallel direct solvers

This repository is an integral part of the bachelor thesis **Parallel reordering algorithms and parallel direct solvers**. It provides an environment for running parallel numerical experiments, demonstrating effect of different reordering algorithms (e.g. AMD, ND, METIS, ...) along with different direct solvers (MUMPS, SuperLU_DIST, PaStiX and MKL_PARDISO), on computational time, fill-in, memory requirements and other relevant metrics.

The results are presented in the **Numerical experiments** chapter of the aforementioned thesis.

**Note:** The installation steps below are intended for a local machine. However, the main results were run on the [LUMI supercomputer](https://lumi-supercomputer.eu/), a HPC (high performance computing) system hosted by CSC in Finland.

## Repository overview and structure
```
numerical_experiments/
│
├── matrices/                 
│   ├── mtx/             # here the matrices in .mtx format should be placed
│   ├── bin/             # here the converted matrix will be stored
│   ├── generate_poisson.edp             # script to generate 3D Poisson problem using FreeFEM
│   ├── mm2petsc.py      # Python script for .mtx to .bin conversion
│   └── matrices.md
│
├── plots/
│   ├── numerical_experiments.ipynb    # plots for the numerical experiments
│   ├── sparsity_structures/ 
│   └── sparsity.ipynb    # scripts for plots of the sparsity structures
│
├── results/
│   ├── mumps/
│   ├── pastix/
│   ├── superlu_dist/
│   ├── pardiso/
│   └── results.ipynb  # batch run experiments and obtain results
│
│── src/
│   ├── experiments.c  # the main experiments driver
│   ├── io.c           # loading, generating and saving data
│   ├── Makefile
│   ├── metrics.c      # obtaining basic statistics
│   ├── reorder.c      # performing reordering
│   ├── solve.c        # solving the system 
│   └── solver_utils.h # all available functions headers
│── .gitignore
└── README.md
```

`mm2petsc.py` is adapted from [Jakub Kružík](https://github.com/jkruzik/nla2/tree/main/tools/mm2petsc)

## Requirements
- [**PETSc**](https://petsc.org/release/)
  built with:
  - MPI support
  - BLAS / LAPACK
  - all relevant direct solvers (MUMPS, SuperLU_DIST, PaStiX)
  - all relevant reorderings (for other related dependencies, see PETSc documentation)

- **MPI implementation**  
  - [OpenMPI](https://www.open-mpi.org/) or [MPICH](https://www.mpich.org/)  

- [**MKL PARDISO**](https://www.intel.com/content/www/us/en/docs/onemkl/developer-reference-c/2023-0/onemkl-pardiso-parallel-direct-sparse-solver-iface.html)
  - Intel oneAPI Base Toolkit (MKL)
  - Intel oneAPI HPC Toolkit (Intel MPI)
  - currently a separate PETSc build is configured with MKL (`arch-mklp`)

- **C compiler**  
  - GCC ≥ 9, Clang ≥ 12, or Intel oneAPI  

- **Python 3 + Jupyter + Matplotlib + SciPy + petsc4py**: for batch running (see results.ipynb and mtx_to_bin.ipynb)


## Installation

Clone the repository:
```bash
git clone https://github.com/axbluefish/parallel_reordering_algorithms.git
cd numerical_experiments
```

Then build with:
```bash
cd src
make
```

This will produce the `experiment` file, which is the main executable for the project.


To build the PARDISO-enabled executable (`experiment_pardiso`), make sure Intel oneAPI is sourced and run:
```bash
make pardiso
```

This uses the `arch-mklp` PETSc build and Intel MPI instead of the default build.

**Note**: The PETSc archs can be build as follows:

```
export PETSC_DIR=`pwd`
export PETSC_ARCH=arch-opt

./configure --with-debugging=0 COPTFLAGS='-O3 -march=native -mtune=native' CXXOPTFLAGS='-O3 -march=native -mtune=native' FOPTFLAGS='-O3 -march=native -mtune=native' --download-mpich --download-fblaslapack --download-scalapack –download-bison --download-metis --download-parmetis --download-mumps --download-superlu --download-superlu_dist --download-hwloc --download-ptscotch --download-pastix –download-netlib-lapack --with-netlib-lapack-c-bindings --download-hypre –with-fortran-bindings=0
```

and 

```
PETSC_ARCH=arch-mkl-pardiso ./configure --with-debugging=0 COPTFLAGS='-O3 -march=native -mtune=native' CXXOPTFLAGS='-O3 -march=native -mtune=native' FOPTFLAGS='-O3 -march=native -mtune=native' --with-mpi-dir=<your mpi directory> --with-blaslapack-dir=$MKLROOT --with-mkl_pardiso=1 --with-mkl_cpardiso=1 --with-mkl_cpardiso-dir=$MKLROOT  --with-mkl_pardiso-dir=$MKLROOT --download-scalapack --with-fortran-bindings=0
```


## Running

After successfully building the project, follow these steps to obtain results: 

1. **Download test matrices** \
Place `.mtx` files into `matrices/mtx/`.

2. **Convert to PETSc binary**
Run the scripts in `matrices/mtx_to_bin.ipynb`. This will convert the `.mtx` matrices into `.bin` format and place them into `matrices/bin` subfolder.

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

