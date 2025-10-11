# === PETSc Paths ===
PETSC_DIR  = ../petsc
PETSC_ARCH = arch-linux-c-opt

# === Compiler and Flags ===
CC        = ${PETSC_DIR}/${PETSC_ARCH}/bin/mpicc
CFLAGS    = -O2 -Wall -Wextra -I${PETSC_DIR}/include -I${PETSC_DIR}/${PETSC_ARCH}/include
LDFLAGS   = -L${PETSC_DIR}/${PETSC_ARCH}/lib -lpetsc -Wl,-rpath,${PETSC_DIR}/${PETSC_ARCH}/lib

# === Files ===
SRC       = experiments.c loading.c profiling.c reorder.c solve.c
OBJ       = $(SRC:.c=.o)
EXE       = experiment

# === Targets ===
.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) *.o
