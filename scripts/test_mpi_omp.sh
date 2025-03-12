#!/bin/bash

#SBATCH --job-name=LSHSJ_mpiomp
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=8
#SBATCH -o ./logs/out_mpi_omp.log
#SBATCH -e ./logs/err_mpi_omp.log
#SBATCH -t 02:00:00

# Usage info: 
#   change --nodes and --ntask-per-node and -n to set the desidered number of process 
#   change --cpus-per-task and OMP_NUM_THREADS to set the desidered number of threads 
#   mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 path_to/executable_filename path_to/dataset_filename path_to/output_filename

# RUN EXAMPLE TEST on different-sized datasets with: 8 NODE, 2 PROCESS PER NODES, 8 THREADS PER PROCESS
mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat
mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh5GB.dat outputs/out_lsh5GB.dat
mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh10GB.dat outputs/out_lsh10GB.dat

# TEST - PAR(1) 
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1 --cpus-per-task=1
#mpirun -x OMP_NUM_THREADS=1 --bynode --bind-to none -n 1 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp_par1.csv
#mpirun -x OMP_NUM_THREADS=1 --bynode --bind-to none -n 1 build/LSHSJ_mpi_omp datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/mpi_omp_par1.csv
#mpirun -x OMP_NUM_THREADS=1 --bynode --bind-to none -n 1 build/LSHSJ_mpi_omp datasets/lsh10GB.dat outputs/out_lsh10GB.dat >> results/mpi_omp_par1.csv

# TEST - Strong Performance analysis on 1GB dataset 
#   --> change sbatch and use
#   --cpus-per-task=2
#mpirun -x OMP_NUM_THREADS=2 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv
#   --> change sbatch and use
#   --cpus-per-task=4
#mpirun -x OMP_NUM_THREADS=4 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv
#   --> change sbatch and use
#   --cpus-per-task=8
#mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv
#   --> change sbatch and use
#   --cpus-per-task=8
#mpirun -x OMP_NUM_THREADS=8 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv


# TEST - Weak Performance analysis 
#   --> change sbatch and use
#   --cpus-per-task=1
#mpirun -x OMP_NUM_THREADS=1 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv
#   --> change sbatch and use
#   --cpus-per-task=5
#mpirun -x OMP_NUM_THREADS=5 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/mpi_omp.csv
#   --> change sbatch and use
#   --cpus-per-task=10
#mpirun -x OMP_NUM_THREADS=10 --bynode --bind-to none -n 16 build/LSHSJ_mpi_omp datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_omp.csv
