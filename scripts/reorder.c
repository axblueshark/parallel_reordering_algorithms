#include "solver_utils.h"

/**
 * @brief Function to get a reordering of the system matrix and right-hand side vector.
 * 
 * @param A The system matrix.
 * @param b The right-hand side vector.
 * @param ordering The ordering type.
 * @param A_perm The permuted matrix.
 * @param b_perm The permuted right-hand side vector.
 * @return PetscErrorCode 
 */
PetscErrorCode reorder( Mat A, Vec b, MatOrderingType ordering, 
                        Mat *A_perm, Vec *b_perm, IS *rperm ) 
{
    IS cperm;

    PetscFunctionBeginUser;

    // get the permutation indices
    PetscCall( MatGetOrdering(A, ordering, rperm, &cperm) );

    // get the permuted matrix
    PetscCall( MatPermute(A, *rperm, cperm, A_perm) );

    // permute rhs
    PetscCall( VecDuplicate(b, b_perm) );
    PetscCall( VecCopy(b, *b_perm) );
    PetscCall( VecPermute(*b_perm, *rperm, PETSC_FALSE) );

    // cleanup
    PetscCall( ISDestroy(&cperm) );

    PetscFunctionReturn( PETSC_SUCCESS );
}