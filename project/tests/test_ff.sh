#!/bin/bash

#SBATCH --job-name=LSHSJ_ff
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH -o ./logs/out_ff.log
#SBATCH -e ./logs/err_ff.log
#SBATCH -t 02:00:00

cd ".."

# Usage info: 
#   path_to/executable_filename num_mapper_threads num_reducer_threads policy path_to/dataset_filename path_to/output_filename
#   policy 0: round-robin - 1: on-demand

# FF mapping string 
ff_dir="fastflow/ff"
cd $ff_dir
echo "Y" | ./mapping_string.sh
cd "../.."

# FF flags config. 0: defaults mapping - blocking mode
make clean
make

# RUN EXAMPLE TEST with 16 total threads 

build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat
build/LSHSJ_ff datasets/lsh10GB.dat 8 8 0 outputs/out_lsh10GB.dat
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat
build/LSHSJ_ff datasets/lsh10GB.dat 8 8 1 outputs/out_lsh10GB.dat

# TEST FF - Weak Performance analysis

#build/LSHSJ_ff datasets/lsh1GB.dat 1 1 0 outputs/out_lsh1GB.dat >> results/ff_weak.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 1 1 1 outputs/out_lsh1GB.dat >> results/ff_weak.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 5 5 0 outputs/out_lsh5GB.dat >> results/ff_weak.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 5 5 1 outputs/out_lsh5GB.dat >> results/ff_weak.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 10 10 0 outputs/out_lsh10GB.dat >> results/ff_weak.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 8 12 0 outputs/out_lsh10GB.dat >> results/ff_weak.csv

# TEST FF - Strong Performance analysis

#build/LSHSJ_ff datasets/lsh1GB.dat 1 1 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 2 2 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 4 4 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 12 12 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 16 16 0 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 1 1 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 2 2 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 4 4 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 12 12 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh1GB.dat 16 16 1 outputs/out_lsh1GB.dat >> results/ff_strong.csv

#build/LSHSJ_ff datasets/lsh5GB.dat 1 1 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 2 2 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 4 4 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 12 12 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 16 16 0 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 1 1 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 2 2 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 4 4 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 12 12 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh5GB.dat 16 16 1 outputs/out_lsh5GB.dat >> results/ff_strong.csv

#build/LSHSJ_ff datasets/lsh10GB.dat 1 1 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 2 2 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 4 4 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 8 8 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 12 12 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 16 16 0 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 1 1 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 2 2 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 4 4 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 8 8 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 8 16 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv
#build/LSHSJ_ff datasets/lsh10GB.dat 8 24 1 outputs/out_lsh10GB.dat >> results/ff_strong.csv