# === PETSc Paths ===
PETSC_DIR  ?= ../petsc
PETSC_ARCH ?= arch-linux-c-opt

# === MKL / Intel MPI ===
MKLROOT    ?= /opt/intel/oneapi/mkl/latest
I_MPI_ROOT ?= /opt/intel/oneapi/mpi/latest

# === Compiler selection ===
ifeq ($(PETSC_ARCH), arch-mklp)
    CC = $(I_MPI_ROOT)/bin/mpicc
else
    CC = $(PETSC_DIR)/$(PETSC_ARCH)/bin/mpicc
endif

# === Compiler Flags ===
CFLAGS  = -O2 -Wall -Wextra \
          -I$(PETSC_DIR)/include \
          -I$(PETSC_DIR)/$(PETSC_ARCH)/include

# === Linker Flags ===
LDFLAGS = -L$(PETSC_DIR)/$(PETSC_ARCH)/lib -lpetsc \
          -Wl,-rpath,$(PETSC_DIR)/$(PETSC_ARCH)/lib

# === Files ===
SRC = experiments.c io.c metrics.c reorder.c solve.c
EXE = experiment

# === Targets ===
.PHONY: all pardiso clean _build

all:
	$(MAKE) PETSC_ARCH=arch-linux-c-opt _build EXE_SUFFIX=""

pardiso:
	$(MAKE) PETSC_ARCH=arch-mklp _build EXE_SUFFIX="_pardiso"

_build:
	$(CC) $(CFLAGS) $(SRC) -o $(EXE)$(EXE_SUFFIX) $(LDFLAGS)

clean:
	rm -f $(EXE) $(EXE)_pardiso *.o