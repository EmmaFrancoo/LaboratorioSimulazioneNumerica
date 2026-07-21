#include "mpi.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    int size, rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 4)
    {
        cout << "Servono 4 processi, non " << size << "!!!" << endl;
        MPI_Finalize();
        return 1;
    }

    int icolor, ikey;

    if (rank == 0)
    {
        icolor = 1;
        ikey = 2;
    }

    if (rank == 1)
    {
        icolor = 1;
        ikey = 1;
    }

    if (rank == 2)
    {
        icolor = 2;
        ikey = 1;
    }

    if (rank == 3)
    {
        icolor = 2;
        ikey = 2;
    }

    MPI_Comm nuovocom;

    MPI_Comm_split(
        MPI_COMM_WORLD,
        icolor,
        ikey,
        &nuovocom
    );

    int newsize, newrank;

    MPI_Comm_size(nuovocom, &newsize);
    MPI_Comm_rank(nuovocom, &newrank);

    cout << "Ero: " << rank << " di " << size
         << " ... e adesso sono: " << newrank
         << " di " << newsize << endl;

    MPI_Comm_free(&nuovocom);
    MPI_Finalize();

    return 0;
}