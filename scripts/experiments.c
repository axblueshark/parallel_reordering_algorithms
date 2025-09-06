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

    const char *input_file = "../matrices/bin/01_s_bcsstk03.bin";
    const char *input_vec_file = "../matrices/bin/09_u_powersim_b.bin";

    Mat A = load_matrix(input_file);
    Vec v = load_vector(input_vec_file);
    PetscInt m, n;
    MatInfo info;

    PetscCall( MatGetSize(A, &m, &n) );  // global matrix size
    PetscCall( MatGetInfo(A, MAT_GLOBAL_SUM, &info) );  // fill the info struct

    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Matrix size: %d x %d\n", m, n) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Number of nonzeros: %.0f\n", info.nz_allocated) );
    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Memory used: %.2f MB\n", info.memory) );

    PetscInt vec_size;

    PetscCall( VecGetSize(v, &vec_size) );

    PetscCall( PetscPrintf(PETSC_COMM_WORLD, "Vec size: %d\n", vec_size) );


    PetscInt nprint = vec_size < 3 ? vec_size : 3;
    PetscInt idx[3] = {0, 1, 2};  // indices we want to read
    PetscScalar vals[3];

    PetscCall(VecGetValues(v, nprint, idx, vals));
    for (PetscInt k = 0; k < nprint; k++) {
        PetscCall(PetscPrintf(PETSC_COMM_WORLD,
            "v[%" PetscInt_FMT "] = %g\n", idx[k]+1, (double)PetscRealPart(vals[k])));
    }

    VecDestroy(&v);
    MatDestroy(&A);

    PetscFinalize();
    return 0;
}
