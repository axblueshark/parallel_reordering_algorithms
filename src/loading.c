#include "solver_utils.h"

/**
 * @brief Loads the system matrix from an input .bin file.
 * 
 * @param filename Path to the input file.
 * @param A        Matrix to load the data into.
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

    // create and load the vector
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
 * @return PetscErrorCode 
 */
PetscErrorCode generate_rhs( Mat A, Vec *b )
{
    Vec x;

    PetscCall( MatCreateVecs(A, &x, b) );
    PetscCall( VecSet(x, 1.0) );
    PetscCall( VecAssemblyBegin(x) ); 
    PetscCall( VecAssemblyEnd(x) );

    PetscCall( MatMult(A, x, *b) );
    PetscCall( VecDestroy(&x) );

    PetscFunctionReturn( PETSC_SUCCESS );
}