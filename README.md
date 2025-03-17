> University of Pisa \
> Computer Science Department \
> M.Sc. Computer science - Artificial Intelligence

# Parallel and Distributed Systems: Paradigms and Models

**Professor:**  M. Torquati, M. Danelutto \
**Academic Year:** 2023/2024 

## LSH-based Similarity Join

### Objectives

This project focus on the Locality Sensitive Hashing-based Similarity Join (LSHSJ) algorithm. The main aim was to provide several parallel versions of the algorithms using:

* **FastFlow (FF)** library to enable threads-level parallelization on Shared-Memory systems such as a sigle multicore node.
* **Message Passing Interface (MPI)** library to enable processes-level parallelization on Distributed-Memory Systesm such us a cluster of several multicore nodes.
* **Threaded MPI with OpenMP (MPI + OpenMP)** to enable parallelization at both threads and processes levels for both a single multi-core node and a clusters of several multicore nodes.

A comprehensive analysis was addressed to explore performances, limitations, and advantages of each adpoted parallelization strategy. All the project phases were in-depth described and discussed in [LSHSJ_reports.pdf](./docs/LSHSJ_report.pdf).
  
### Structure

```bash
 /project
   │
   ├── build
   │   └── ...                # Executables files
   ├── src
   │   ├── lshsj_ff.cpp       # Source code for FF version
   │   ├── lshsj_mpi.cpp      # Source code for MPI version
   │   ├── lshsj_mpi_omp.cpp  # Source code for MPI + OMP version
   │   ├── lshsj_mpi_nb.cpp   # Source code for non-blocking MPI version
   │   └── lshsj_seq.cpp      # Source code for sequential version
   ├── logs       
   │   └── ...                # Logs and error files from SLURM
   ├── dependencies      
   │   └── ...                # Source code for Frechet distance and geometric ops
   ├── fastflow      
   │   └── ...                # FF header library (must be installed) 
   ├── outputs
   │   └── ...                # Output files (with founded similar pairs) 
   ├── results
   │   └── ...                # Results file (performance evaluations, metrics, completion time...)   
   ├── datasets
   │   ├── taxi1.dat          # Tiny dataset for quick testing
   │   ├── taxi2.dat          # Tiny dataset for quick testing
   │   ├── lsh1GB.dat         # Small synthetic dataset (to be generated)
   │   ├── lsh5GB.dat         # Medium synthetic dataset (to be generated)
   │   └── lsh10GB.dat        # Large synthetic dataset (to be generated)
   ├── plots      
   │   └── LSHSJ_plots.ipynb  # Python notebook for plots generations
   ├── tests                  # Bash files for performance evaluation with SLURM    
   │   ├── test_seq.sh
   │   ├── test_mpi.sh
   │   ├── test_ff.sh
   │   └── ...
   ├── MakeFile               # Makefile for compiling the source code
   └── setup.sh               # Set up instructions and install requirements
```

### Setup and Usage 

Clone current repository
   ```bash
   git clone https://github.com/irenepisani/ParallelDistributedSystems_LSHSJ.git
   cd ParallelDistributedSystems_LSHSJ/project
   ```
Install requirements and build the project as follow: 
   ```bash
   bash setup.sh
   ```
Ensure SLURM is available on the used clusters and submit job-experiments with commands: 

   ```bash
   cd tests
   sbatch test_seq.sh  # run sequential test
   sbatch test_ff.sh   # run FF test
   sbatch test_mpi.sh  # run MPI test
   sbatch ...          # run other tests ... 
   ```
### Datasets and Framework parameters

Change framework paramaters directly in source code in order to set desidered values based on the used datasets. 
- **Tiny Datasets**: `taxi1.dat`, `taxi2.dat` for quick testing.
  - Threshold (λ): 0.08
  - LSH Resultion: 0.01
- **Synthetic Datasets**: `lsh1GB.dat`, `lsh5GB.dat`, `lsh10GB.dat` for performance evaluation.
  - [Dataset generation](https://github.com/nicolotonci/FF-LSHSJ/blob/fc0c22c3cfb8d17c70468d4672d3bf791dae66bf/examples/Frechet/datasets/generate.py)
  - Threshold (λ): 80
  - LSH Resultion: 10

---

**Author:**  [Irene Pisani](mailto:i.pisani1@studenti.unipi.it) \
This project is licensed under the MIT License. 
See the `LICENSE` file for details.


