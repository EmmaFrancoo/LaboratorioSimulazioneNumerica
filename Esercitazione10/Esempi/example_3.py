from mpi4py import MPI 
import numpy as np

comm = MPI.COMM_WORLD 
size = comm.Get_size() 
rank = comm.Get_rank()

if size > 3:
    exit('Hai scelto troppi processi')

irecv = np.zeros(3)
isend = np.zeros(1)
isend[0] = rank+1

comm.Gather(isend, irecv, root=0)

if rank == 0:
    print('irecv: ', irecv[0], ' ', irecv[1], ' ', irecv[2])