#include "solver_utils.h"

/**
 * @brief Loads the system matrix from an input .bin file.
 * 
 * @param filename Path to the input file.
 * @param A        Matrix to load the data into.
 * @param mat_type Matrix type (MATSEQAIJ or MATMPIAIJ).
 * @return PetscErrorCode 
 */
PetscErrorCode load_matrix ( const char *filename, Mat *A )
{
    PetscViewer viewer;

    PetscFunctionBeginUser;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( MatCreate(PETSC_COMM_WORLD, A) );
    PetscCall( MatSetFromOptions(*A) );
    PetscCall( MatLoad(*A, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    PetscFunctionReturn( PETSC_SUCCESS );
}


/**
 * @brief Loads the RHS vector from .bin file.
 * 
 * @param filename Path to the input file.
 * @param v        Vector to load the data into.
 * @return PetscErrorCode 
 */
PetscErrorCode load_vector( const char *filename, Vec *v )
{
    PetscViewer viewer;

    PetscFunctionBeginUser;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( VecCreate(PETSC_COMM_WORLD, v) );
    PetscCall( VecSetFromOptions(*v) );
    PetscCall( VecLoad(*v, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    PetscFunctionReturn( PETSC_SUCCESS );
}

/**
 * @brief Generates an all-ones RHS (if no RHS is available).
 * 
 * @param A        System matrix for which the RHS is to be generated.
 * @param b        Vector to be generated.
 * @param vec_type Vector type (VECSEQ or VECMPI).
 * @return PetscErrorCode 
 */
PetscErrorCode generate_rhs( Mat A, Vec *b )
{
    Vec      ones;
    PetscInt n;

    PetscCall( MatGetSize(A, NULL, &n) );

    // create a vector filled with ones
    PetscCall( VecCreate(PETSC_COMM_WORLD, &ones) );
    PetscCall( VecSetSizes(ones, PETSC_DECIDE, n) );
    PetscCall( VecSetFromOptions(ones) );
    PetscCall( VecSet(ones, 1.0) );
    PetscCall( VecAssemblyBegin(ones) );
    PetscCall( VecAssemblyEnd(ones) );

    // create b of size n and calculate b = A * ones
    PetscCall( VecDuplicate(ones, b) );
    PetscCall( MatMult(A, ones, *b) );

    PetscCall( VecDestroy(&ones) );
    
    PetscFunctionReturn( PETSC_SUCCESS );
}