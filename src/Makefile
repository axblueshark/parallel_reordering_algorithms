PETSC_DIR  = ../petsc
PETSC_ARCH = arch-opt

include $(PETSC_DIR)/lib/petsc/conf/variables

SRC = experiments.c io.c metrics.c reorder.c solve.c
OBJ = experiments.o io.o metrics.o reorder.o solve.o
EXE = experiment

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	@echo "Linking $(EXE)..."
	$(PCC) $(PETSC_CC_FLAGS) -o $(EXE) $(OBJ) $(PETSC_LIB)

%.o: %.c
	@echo "Compiling $<..."
	$(PCC) $(PETSC_CC_INCLUDES) $(PETSC_CC_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXE)