#ifndef SOLVER_UTILS_H
#define SOLVER_UTILS_H

#include <petsc.h>

// -------------------------------------
// MATRIX AND VECTOR LOADING/GENERATING
// -------------------------------------

/**
 * @brief Loads the system matrix from an input .bin file.
 * 
 * @param filename Path to the input file.
 * @param A        Matrix to load the data into.
 * @param mat_type Matrix type (MATSEQAIJ or MATMPIAIJ).
 * @return PetscErrorCode 
 */
PetscErrorCode load_matrix ( const char *filename, Mat *A );

/**
 * @brief Loads the RHS vector from .bin file.
 * 
 * @param filename Path to the input file.
 * @param v        Vector to load the data into.
 * @return PetscErrorCode 
 */
PetscErrorCode load_vector ( const char *filename, Vec *v );

/**
 * @brief Generates an all-ones RHS (if no RHS is available).
 * 
 * @param A        System matrix for which the RHS is to be generated.
 * @param b        Vector to be generated.
 * @param vec_type Vector type (VECSEQ or VECMPI).
 * @return PetscErrorCode 
 */
PetscErrorCode generate_rhs( Mat A, Vec *b );


// -------------------------------------
// REORDERING
// -------------------------------------

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
                        Mat *A_perm, Vec *b_perm, IS *rperm );


// -------------------------------------
// SYSTEM SOLVER
// -------------------------------------

/**
 * @brief Function to solve the system of linear equations Ax = b.
 * 
 * @param A The system matrix.
 * @param b The right-hand side vector.
 * @param x The solution vector.
 * @param ordering_type The type of the reordering to be used.
 * @param pc_type The type of the preconditioner to be used.
 * @param mat_solver_type The solver type.
 * @return PetscErrorCode 
 */
PetscErrorCode solve_system( Mat A, Vec b, Vec *x,
                             PCType pc_type, 
                             MatSolverType mat_solver_type,
                             PetscLogStage stage_factor,
                             PetscLogStage stage_solve );


// -------------------------------------
// METRICS AND PROFILING
// -------------------------------------

/**
 * @brief Print the main info about a given matrix.
 * 
 * @param A The matrix.
 * @return PetscErrorCode 
 */
PetscErrorCode matrix_info( Mat A );


/**
 * @brief Save a matrix to .bin format.
 * 
 * @param A The matrix to be saved.
 * @param path The desired file path.
 * @return PetscErrorCode 
 */
PetscErrorCode save_matrix( Mat A, const char* path );


/**
 * @brief Compute the norm of obtained solution (Ax - b).
 * 
 * @param A System matrix.
 * @param b Right-hand side vector.
 * @param x Obtained solution of the system Ax = b.
 * @param norm The norm to be computed.
 * @return PetscErrorCode 
 */
PetscErrorCode compute_solution_norm( Mat A, Vec b, Vec x, PetscReal *norm );

#endif // SOLVER_UTILS_H
