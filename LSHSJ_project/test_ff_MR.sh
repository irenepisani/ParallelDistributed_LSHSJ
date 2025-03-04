#!/bin/bash

#SBATCH --job-name=LSHSJ_ff_MR
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH -o ./logs/out_ff_MR.log
#SBATCH -e ./logs/err_ff_MR.log
#SBATCH -t 02:00:00

# Usage info: 
#   path_to/executable_filename num_mapper_threads num_reducer_threads policy path_to/dataset_filename path_to/output_filename
#   policy 0: round-robin - 1: on-demand


# FF mapping string 
ff_dir="fastflow/ff"
cd $ff_dir
echo "Y" | ./mapping_string.sh
cd "../.."

# FF MR config. 0: defaults mapping - nonblocking mode
make clean
make

# TEST: Trade off between number of mapper threads and reducer threads

build/LSHSJ_ff datasets/lsh1GB.dat 4 12 0 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 6 10 0 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 10 6 0 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 12 4 0 outputs/out_lsh1GB.dat >> results/ff_MR.csv

build/LSHSJ_ff datasets/lsh1GB.dat 4 12 1 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 6 10 1 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 10 6 1 outputs/out_lsh1GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh1GB.dat 12 4 1 outputs/out_lsh1GB.dat >> results/ff_MR.csv

build/LSHSJ_ff datasets/lsh5GB.dat 4 12 0 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 6 10 0 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 10 6 0 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 12 4 0 outputs/out_lsh5GB.dat >> results/ff_MR.csv

build/LSHSJ_ff datasets/lsh5GB.dat 4 12 1 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 6 10 1 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 10 6 1 outputs/out_lsh5GB.dat >> results/ff_MR.csv
build/LSHSJ_ff datasets/lsh5GB.dat 12 4 1 outputs/out_lsh5GB.dat >> results/ff_MR.csv
