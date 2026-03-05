PETSC_DIR  = ../petsc
PETSC_ARCH ?= arch-opt

include $(PETSC_DIR)/lib/petsc/conf/variables

SRC = experiments.c io.c metrics.c reorder.c solve.c
OBJ = $(SRC:.c=.o)
OBJ_PARDISO = $(SRC:.c=.pardiso.o)
EXE = experiment

.PHONY: all clean pardiso

all:
	@$(MAKE) PETSC_ARCH=arch-opt $(EXE)

pardiso:
	@$(MAKE) PETSC_ARCH=arch-mkl-pardiso experiment_pardiso

$(EXE): $(OBJ)
	@echo "Linking $(EXE) using $(PETSC_ARCH)..."
	$(PCC) $(PETSC_CC_FLAGS) -o $(EXE) $(OBJ) $(PETSC_LIB)

experiment_pardiso: $(OBJ_PARDISO)
	@echo "Linking experiment_pardiso using arch-mkl-pardiso..."
	$(PCC) $(PETSC_CC_FLAGS) -o experiment_pardiso $(OBJ_PARDISO) $(PETSC_LIB)

%.o: %.c
	@echo "Compiling $< for arch-opt..."
	$(PCC) $(PETSC_CC_INCLUDES) $(PETSC_CC_FLAGS) -c $< -o $@

%.pardiso.o: %.c
	@echo "Compiling $< for arch-mkl-pardiso..."
	$(PCC) $(PETSC_CC_INCLUDES) $(PETSC_CC_FLAGS) -c $< -o $@

clean:
	rm -f *.o *.pardiso.o experiment experiment_pardiso