static const char help[] = "Simple converter from .mtx to .bin format (for vectors).\n";

#include <petsc.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
    FILE       *file;
    PetscInt    n, dummy;
    PetscReal   val;
    Vec         v;
    PetscViewer viewer;
    char        line[512];

    PetscCall( PetscInitialize(&argc, &argv, NULL, help) );

    // if some arguments are missing
    if (  argc < 3) {
        PetscPrintf(PETSC_COMM_WORLD,
            "Some arguments are missing, expected format: %s input.mtx output.bin\n", argv[0]);
        PetscFinalize();
        return 1;
    }

    // open input .mtx file
    file = fopen( argv[1], "r" );
    if ( !file ) {
        PetscPrintf( PETSC_COMM_WORLD, "Could not open file: %s\n", argv[1] );
        PetscFinalize();
        return 1;
    }

    // skip comments (headers beginning with %)
    do {
        if ( !fgets(line, sizeof(line), file) ) {
            PetscPrintf( PETSC_COMM_WORLD, "Invalid vector MatrixMarket file.\n" );
            fclose( file );
            PetscFinalize();
            return 1;
        }
    } while ( line[0] == '%' );

    sscanf( line, "%" PetscInt_FMT " %" PetscInt_FMT, &n, &dummy );

    // create empty vector of length n
    PetscCall( VecCreate(PETSC_COMM_WORLD, &v) );
    PetscCall( VecSetType(v, VECMPI) );
    PetscCall( VecSetSizes(v, PETSC_DECIDE, n) );
    PetscCall( VecSetFromOptions(v) );

    // read entries (n values, one per line)
    for ( PetscInt i = 0; i < n; i++ ) {
        if ( fscanf(file, "%lf", &val) != 1 ) {
            PetscPrintf(PETSC_COMM_WORLD, "Failed to read vector entry #%" PetscInt_FMT "\n", i + 1);
            break;
        }
        PetscCall( VecSetValue(v, i, val, INSERT_VALUES) );
    }

    fclose( file );

    PetscCall( VecAssemblyBegin(v) );
    PetscCall( VecAssemblyEnd(v) );

    // save to binary format
    PetscCall( PetscViewerBinaryOpen(PETSC_COMM_WORLD, argv[2], FILE_MODE_WRITE, &viewer) );
    PetscCall( VecView(v, viewer) );
    PetscCall( PetscViewerDestroy(&viewer) );
    PetscCall( VecDestroy(&v) );

    PetscFinalize();
    return 0;
}