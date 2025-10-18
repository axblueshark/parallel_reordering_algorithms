# === PETSc Paths ===
PETSC_DIR  = ../petsc
PETSC_ARCH = arch-linux-c-opt

# === Optional: MKL path (used only for runtime rpath) ===
MKLROOT ?= /opt/intel/oneapi/mkl/2025.2

# === Compiler and Flags (use PETSc's MPI wrapper) ===
CC      = $(PETSC_DIR)/$(PETSC_ARCH)/bin/mpicc
CFLAGS  = -O2 -Wall -Wextra \
          -I$(PETSC_DIR)/include \
          -I$(PETSC_DIR)/$(PETSC_ARCH)/include

# Link to PETSc and embed runtime search paths so you don't need LD_LIBRARY_PATH
LDFLAGS = -L$(PETSC_DIR)/$(PETSC_ARCH)/lib -lpetsc \
          -Wl,-rpath,$(PETSC_DIR)/$(PETSC_ARCH)/lib \
          -Wl,-rpath,$(MKLROOT)/lib

# === Files ===
SRC = experiments.c loading.c profiling.c reorder.c solve.c
OBJ = $(SRC:.c=.o)
EXE = experiment

# === Targets ===
.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) *.o
