#!/bin/bash

#SBATCH --job-name=LSHSJ_mpi
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=2
#SBATCH -o ./logs/out_mpi.log
#SBATCH -e ./logs/err_mpi.log
#SBATCH -t 02:00:00

cd ".."

# Usage info: 
#   change --nodes and --ntask-per-node to set the desidered number of process and nodes
#   srun --mpi=pmix path_to/executable_filename path_to/dataset_filename path_to/output_filename

# RUN EXAMPLE TEST on different-sized datasets with: 8 NODE, 2 PROCESS PER NODES
srun --mpi=pmix build/LSHSJ_mpi datasets/lsh1GB.dat outputs/out_lsh1GB.dat
srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh5GB.dat
srun --mpi=pmix build/LSHSJ_mpi datasets/lsh10GB.dat outputs/out_lsh10GB.dat

# TEST - PAR(1) 
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh1GB.dat >> results/mpi_par1.csv
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh1GB.dat >> results/mpi_par1.csv
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh1GB.dat >> results/mpi_par1.csv

# TEST - Strong analysis on 1GB dataset varying number of node and process per node
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1
#   --nodes=1 --ntasks-per-node=2
#   --nodes=1 --ntasks-per-node=4
#   --nodes=1 --ntasks-per-node=8
#   --nodes=1 --ntasks-per-node=16
#   --nodes=2 --ntasks-per-node=1
#   --nodes=2 --ntasks-per-node=2
#   --nodes=2 --ntasks-per-node=4
#   --nodes=2 --ntasks-per-node=8
#   --nodes=4 --natsks-per-node=1
#   --nodes=4 --natsks-per-node=2
#   --nodes=4 --natsks-per-node=4
#   --nodes=8 --natsks-per-node=1
#   --nodes=8 --natsks-per-node=2
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_strong_1GB.csv

# TEST - Strong analysis on all dataset 1GB
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1
#   --nodes=2 --ntasks-per-node=1
#   --nodes=4 --ntasks-per-node=1
#   --nodes=8 --ntasks-per-node=1
#   --nodes=8 --ntasks-per-node=2
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_strong_1N.csv
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/mpi_strong_1N.csv
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh10GB.dat outputs/out_lsh10GB.dat >> results/mpi_strong_1N.csv

# TEST - Weak analysis varying nodes 
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1
#   --nodes=1 --ntasks-per-node=2
#   --nodes=1 --ntasks-per-node=4
#   --nodes=1 --ntasks-per-node=8
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_weak_N.csv
#   --> change sbatch and use
#   --nodes=4 --ntasks-per-node=1
#   --nodes=4 --ntasks-per-node=2
#   --nodes=4 --ntasks-per-node=4
#   --nodes=4 --ntasks-per-node=8
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/mpi_weak_N.csv
#   --> change sbatch and use
#   --nodes=8 --ntasks-per-node=1
#   --nodes=8 --ntasks-per-node=2
#   --nodes=8 --ntasks-per-node=4
#   --nodes=8 --ntasks-per-node=8
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh10GB.dat outputs/out_lsh10GB.dat >> results/mpi_weak_N.csv

# TEST - Weak analysis varying process
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=1
#   --nodes=2 --ntasks-per-node=1
#   --nodes=4 --ntasks-per-node=1
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/mpi_weak_P.csv
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=5
#   --nodes=2 --ntasks-per-node=5
#   --nodes=4 --ntasks-per-node=5
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/mpi_weak_P.csv
#   --> change sbatch and use
#   --nodes=1 --ntasks-per-node=10
#   --nodes=2 --ntasks-per-node=10
#   --nodes=4 --ntasks-per-node=10
#srun --mpi=pmix build/LSHSJ_mpi datasets/lsh10GB.dat outputs/out_lsh10GB.dat >> results/mpi_weak_P.csv
