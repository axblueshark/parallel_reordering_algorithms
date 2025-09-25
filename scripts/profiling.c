#include "solver_utils.h"

/**
 * @brief Print the main info about a given matrix.
 * 
 * @param A The matrix.
 * @return PetscErrorCode 
 */
PetscErrorCode matrix_info( Mat A )
{
    PetscInt m, n;
    MatInfo  info;
    MatType  type;

    PetscFunctionBeginUser;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );
    PetscCall( MatGetType(A, &type) );

    PetscPrintf( PETSC_COMM_SELF,
        "----------------------------------------\n"
        "Matrix info:\n"
        "- size:               %" PetscInt_FMT " x %" PetscInt_FMT "\n"
        "- type:               %s\n"
        "- nnz (used):         %" PetscInt_FMT "\n"
        "- nnz (allocated):    %" PetscInt_FMT "\n"
        "- sparsity (nnz/n2):  %.5f\n"
        "----------------------------------------\n",
        m, n,
        type,
        (PetscInt)info.nz_used,
        (PetscInt)info.nz_allocated,
        (double)info.nz_used / ((double)m * (double)n)
    );

    PetscFunctionReturn( PETSC_SUCCESS );
}
