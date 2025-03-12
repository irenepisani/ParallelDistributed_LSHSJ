#!/bin/bash

#SBATCH --job-name=LSHSJseq
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH -o ./logs/out_seq.log
#SBATCH -e ./logs/err_seq.log
#SBATCH -t 02:00:00

cd ".."

# Usage info: 
# path_to/executable_filename path_to/dataset_filename path_to/output_filename

# Run sequential test on different sized datasets
build/LSHSJ_seq datasets/lsh1GB.dat outputs/out_lsh1GB.dat >> results/seq.csv
build/LSHSJ_seq datasets/lsh5GB.dat outputs/out_lsh5GB.dat >> results/seq.csv
build/LSHSJ_seq datasets/lsh10GB.dat outputs/out_lsh10GB.dat >> results/seq.csv
