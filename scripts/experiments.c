static const char help[] = "Parallel reordering algorithms and parallel direct solvers.\n";

#include <petsc.h>

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

void apply_reordering(Mat A) 
{
    // TODO: Implement matrix reordering using MatGetOrdering, etc.
}

Vec solve_system(Mat A) 
{
    Vec x;
    // TODO: Create RHS, set up KSP, solve the system, return solution vector
    return x;
}

void save_results(Vec x, const char *output_filename) 
{
    // TODO: Save solution vector to file using PetscViewer
}

// --- Main Function ---

int main( int argc, char **argv )
{
    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    //const char *input_file = "matrix.mtx";       // or pass via -f option - better probably
    const char *output_file = "solution.txt";

    const char *input_file = "../matrices/bin/04_s_barth4_coord.bin";

    Mat A = load_matrix(input_file);


    /*
    PetscBool is_symmetric;
    PetscReal tol = 1e-10; // tolerance for numerical symmetry

    PetscCall(MatIsSymmetric(A, tol, &is_symmetric));

    if (is_symmetric) {
        PetscPrintf(PETSC_COMM_WORLD, "Matrix is symmetric (within tolerance).\n");
    } else {
        PetscPrintf(PETSC_COMM_WORLD, "Matrix is NOT symmetric.\n");
    }*/


    // check if matrix is loaded properly
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );  // global matrix size
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );  // fill the info struct

    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Memory used: %.2f MB\n", info.memory) );

    //apply_reordering(A);

    //Vec x = solve_system(A);
    
    //save_results(x, output_file);

    // cleanup 
    //VecDestroy(&x);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
