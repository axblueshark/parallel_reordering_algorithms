PETSC_DIR = ../petsc
PETSC_ARCH = arch-linux-32-opt

CC = ${PETSC_DIR}/${PETSC_ARCH}/bin/mpicc
CFLAGS = -O2 -Wall -Wextra -I${PETSC_DIR}/include -I${PETSC_DIR}/${PETSC_ARCH}/include
LDFLAGS = -L${PETSC_DIR}/${PETSC_ARCH}/lib -lpetsc -Wl,-rpath,${PETSC_DIR}/${PETSC_ARCH}/lib

SRC = experiments.c loading.c profiling.c reorder.c solve.c
OBJ = $(SRC:.c=.o)

EXE = experiment

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c solver_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXE) *.o
