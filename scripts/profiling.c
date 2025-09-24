#include "solver_utils.h"

/**
 * @brief Print the main info about a given matrix.
 * 
 * @param A Matrix.
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

    PetscPrintf( PETSC_COMM_WORLD,
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
 * @brief Measures the solution time.
 * 
 * @param ksp  KSP context.
 * @param time Time in seconds.
 * @return PetscErrorCode 
 */
PetscErrorCode measure_solve_time( KSP ksp, PetscLogDouble *time )
{

}

/**
 * @brief Measures the memory usage.
 * 
 * @param memory_usage The estimated memroy.
 * @return PetscErrorCode 
 */
PetscErrorCode measure_memory_usage( PetscLogDouble *memory_usage )
{
    
}

/**
 * @brief Computes fill-in ratio: nnz(L+U) / nnz(A)
 * 
 * @param A          System matrix.
 * @param L 
 * @param fill_ratio 
 * @return PetscErrorCode 
 */
PetscErrorCode measure_fill_in( Mat A, Mat L, PetscReal *fill_ratio )
{
    
}

/**
 * @brief Save the obtained results to an output file.
 * 
 * @param x 
 * @param output_filename 
 * @return PetscErrorCode 
 */
PetscErrorCode save_results( Vec x, const char *output_filename)
{
    
}