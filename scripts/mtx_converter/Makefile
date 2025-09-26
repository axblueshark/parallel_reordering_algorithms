PETSC_DIR = ../../petsc
PETSC_ARCH = arch-linux-c-opt

CC = ${PETSC_DIR}/${PETSC_ARCH}/bin/mpicc
CFLAGS = -O2 -Wall -Wextra -I${PETSC_DIR}/include -I${PETSC_DIR}/${PETSC_ARCH}/include
LDFLAGS = -L${PETSC_DIR}/${PETSC_ARCH}/lib -lpetsc -Wl,-rpath,${PETSC_DIR}/${PETSC_ARCH}/lib

SRC_MAT = mtx_to_bin_mat.c
SRC_VEC = mtx_to_bin_vec.c
EXE_MAT = mtx_to_bin_mat
EXE_VEC = mtx_to_bin_vec

.PHONY: all clean

all: $(EXE_MAT) $(EXE_VEC)

$(EXE_MAT): $(SRC_MAT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(EXE_VEC): $(SRC_VEC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXE_MAT) $(EXE_VEC)
