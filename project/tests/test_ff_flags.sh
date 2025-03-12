#!/bin/bash

#SBATCH --job-name=LSHSJ_ff_flags
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH -o ./logs/out_ff_flags.log
#SBATCH -e ./logs/err_ff_flags.log
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

# TEST (1): Concurrency control                    

# FF flags config. 0: defaults mapping - blocking mode
make clean
make
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff.csv

# FF flags config. 1: no defaults mapping - blocking mode
make clean
make MAPPING=1
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm.csv

# FF flags config. 3: defaults mapping - non blocking mode
make clean
make BLOCKING=1
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nb.csv

# FF flags config. 2: no defaults mapping - non blocking mode
make clean
make MAPPING=1 BLOCKING=1
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 0 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 4 4 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 8 8 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh1GB.dat 16 16 1 outputs/out_lsh1GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 0 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 4 4 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 8 8 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv
build/LSHSJ_ff datasets/lsh5GB.dat 16 16 1 outputs/out_lsh5GB.dat >> results/res_ff_flags/ff_nm_nb.csv