# Parallel reordering algorithms and parallel direct solvers

description & features TBA

## Repository overview and structure
```
numerical_experiments/
│
├── matrices/
│   ├── mtx/
│   ├── bin/
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
│   └── results.ipynb
│
│── scripts/
│   ├── mtx_converter/
│   ├── experiments.c
│   ├── loading.c
│   ├── Makefile
│   ├── profiling.c
│   ├── reorder.c
│   ├── solve.c
│   └── solver_utils.h
│── .gitignore
└── README.md
```

## Requirements

## Installation
```bash
cd scripts
make
```



## Run example
```bash
mpirun -n 4 ./experiment \
  -mat_file ../matrices/bin/01_s_ex2.bin \
  -pc_type lu \
  -mat_solver_type mumps \
  -mat_ordering_type amd \
  -own_reordering true \
  -get_solution_norm \
  -log_view
```

