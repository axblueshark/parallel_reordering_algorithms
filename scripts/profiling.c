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

/**
 * @brief Save a matrix to .bin format.
 * 
 * @param A The matrix to be saved.
 * @param path The desired file path.
 * @return PetscErrorCode 
 */
PetscErrorCode save_matrix( Mat A, const char* path )
{
    PetscViewer viewer;

    PetscFunctionBeginUser;

    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, path, FILE_MODE_WRITE, &viewer) );
    PetscCall( MatView(A, viewer) );
    PetscCall( PetscViewerDestroy(&viewer) );

    PetscFunctionReturn( PETSC_SUCCESS );
}


/**
 * @brief Compute the norm of obtained solution (Ax - b).
 * 
 * @param A System matrix.
 * @param b Right-hand side vector.
 * @param x Obtained solution of the system Ax = b.
 * @param norm The norm to be computed.
 * @return PetscErrorCode 
 */
PetscErrorCode compute_solution_norm( Mat A, Vec b, Vec x, PetscReal *norm )
{
    Vec r;

    PetscFunctionBeginUser;

    PetscCall( VecDuplicate(b, &r) );        // allocate r
    PetscCall( MatMult(A, x, r) );           // r = A * x
    PetscCall( VecAXPY(r, -1.0, b) );        // r = r - b = Ax - b
    PetscCall( VecNorm(r, NORM_2, norm) );  // norm = ||r||_2

    PetscCall( VecDestroy(&r) );

    PetscFunctionReturn( PETSC_SUCCESS );
}