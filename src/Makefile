# === PETSc Paths ===
PETSC_DIR  = ../petsc
PETSC_ARCH ?= arch-linux-c-opt

# === MKL / Intel MPI ===
MKLROOT    ?= /opt/intel/oneapi/mkl/2025.3
I_MPI_ROOT ?= /opt/intel/oneapi/mpi/2021.16

# === Compiler and arch-specific flags ===
ifeq ($(PETSC_ARCH), arch-mklp)
    CC           = $(I_MPI_ROOT)/bin/mpigcc
    MPI_INCLUDES = -I$(I_MPI_ROOT)/include
    MPI_LDFLAGS  = -L$(I_MPI_ROOT)/lib -L$(I_MPI_ROOT)/lib/release \
                   -Wl,-rpath,$(I_MPI_ROOT)/lib \
                   -Wl,-rpath,$(I_MPI_ROOT)/lib/release \
                   -L$(MKLROOT)/lib \
                   -lmkl_blacs_intelmpi_lp64 \
                   -Wl,-rpath,$(MKLROOT)/lib
else
    CC           = $(PETSC_DIR)/$(PETSC_ARCH)/bin/mpicc
    MPI_INCLUDES =
    MPI_LDFLAGS  =
endif

# === Compiler Flags ===
CFLAGS  = -O2 -Wall -Wextra \
          -I$(PETSC_DIR)/include \
          -I$(PETSC_DIR)/$(PETSC_ARCH)/include \
          $(MPI_INCLUDES)

# === Linker Flags ===
LDFLAGS = -L$(PETSC_DIR)/$(PETSC_ARCH)/lib -lpetsc \
          -Wl,-rpath,$(PETSC_DIR)/$(PETSC_ARCH)/lib \
          $(MPI_LDFLAGS)

# === Files ===
SRC = experiments.c loading.c profiling.c reorder.c solve.c
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