static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>

/**
 * @brief Loads the system matrix from an input .bin file.
 * 
 * @param filename The input file.
 * @return Mat 
 */
Mat load_matrix( const char *filename )
{
    Mat         A;
    PetscViewer viewer;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( MatCreate(PETSC_COMM_WORLD, &A) );
    PetscCall( MatSetFromOptions(A) );
    PetscCall( MatLoad(A, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    return A;
}

/**
 * @brief Loads the RHS vector from an input .bin file.
 * 
 * @param filename The input file.
 * @return Vec 
 */
Vec load_vector( const char *filename )
{
    Vec         v;
    PetscViewer viewer;

    // open PETSc binary file for reading
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, filename, FILE_MODE_READ, &viewer) );

    // create and load the matrix
    PetscCall( VecCreate(PETSC_COMM_WORLD, &v) );
    PetscCall( VecSetFromOptions(v) );
    PetscCall( VecLoad(v, viewer) );

    // cleanup
    PetscCall( PetscViewerDestroy(&viewer) );

    return v;
}

/**
 * @brief Function to generate right hand vector if none is available from suite sparse collection.
 * 
 * @param A The system matrix.
 * @return Vec 
 */
Vec generate_rhs( Mat A )
{
    Vec      b, ones;
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
    PetscCall( VecDuplicate(ones, &b) );
    PetscCall( MatMult(A, ones, b) );

    PetscCall( VecDestroy(&ones) );
    return b;
}

/**
 * @brief Function to get a reordering of the system matrix and right-hand-side vector.
 * 
 * @param A The system matrix.
 * @param b The right-hand-side vector.
 * @param ordering The ordering type.
 * @param A_perm The permuted matrix.
 * @param b_perm The permuted right-hand-side vector.
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

Vec solve_system( Mat A, Vec b, 
                  MatOrderingType ordering_type,
                  PCType pc_type, MatSolverType mat_solver_type
                )
{
    // variables setup
    KSP ksp;
    PC  pc;
    IS  rperm;
    Vec x, b_perm;
    Mat A_perm;

    // reordering
    PetscCall( reorder(A, b, ordering_type, &A_perm, &b_perm, &rperm) );

    // solver setup
    PetscCall( KSPCreate(PETSC_COMM_WORLD, &ksp) );
    PetscCall( KSPSetOperators(ksp, A_perm, A_perm) );
    PetscCall( KSPSetType(ksp, KSPPREONLY) ); // to use direct solvers
    
    PetscCall( KSPGetPC(ksp, &pc) );
    PetscCall( PCSetType(pc, pc_type) );
    PetscCall( PCFactorSetMatSolverType(pc, mat_solver_type) );
    PetscCall( PCFactorSetUpMatSolverType(pc) );

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );

    // allocate space for solution vector
    PetscCall( VecDuplicate(b_perm, &x) );

    // solve
    PetscCall( KSPSolve(ksp, b_perm, x) );

    // "unpermute" the solution to get the original one 
    PetscCall( VecPermute(x, rperm, PETSC_TRUE) );

    // TODO: error

    // cleanup
    PetscCall( KSPDestroy(&ksp) );
    PetscCall( MatDestroy(&A_perm) );
    PetscCall( VecDestroy(&b_perm) );
    PetscCall( ISDestroy(&rperm) );

    return x;
}


void save_results(Vec x, const char *output_filename) 
{
    // TODO: Save solution vector to file using PetscViewer
}

//========================================================
int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );


    const char *input_mat_file = "../matrices/bin/03_s_ex10hs.bin";
    const char *input_rhs_file = "../matrices/bin/09_u_powersim_b.bin";

    Mat A = load_matrix(input_mat_file);
    Vec b = input_rhs_file != "" ? load_vector(input_rhs_file) : generate_rhs(A);

    // check if loaded correctly
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );

    //PetscInt vec_size;
    //PetscCall( VecGetSize(v, &vec_size) );
    //PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Vec size: %" PetscInt_FMT "\n", vec_size) );
//
    //PetscInt nprint = 3;
    //PetscInt idx[3] = {0, 1, 2};
    //PetscScalar vals[3];

    //PetscCall( VecGetValues(v, nprint, idx, vals) );
    //for ( PetscInt i = 0; i < nprint; i++ ) {
    //    PetscCall( PetscPrintf(PETSC_COMM_WORLD,
    //        "v[%" PetscInt_FMT "] = %g\n", idx[i] + 1, (double)PetscRealPart(vals[i])) );
    //}

    Vec x = solve_system(
        A, b,
        MATORDERINGNATURAL,
        PCLU,
        MATSOLVERSUPERLU
    );

    VecDestroy(&x);
    VecDestroy(&b);
    //VecDestroy(&x);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
