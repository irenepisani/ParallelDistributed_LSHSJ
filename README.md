> University of Pisa, Computer Science Department \
> M.Sc. Computer science, Artificial Intelligence


# Parallel and Distributed Systems: Paradigms and Models

**Professor:** M. Torquati, M. Danelutto \
**Academic year:** 2023/2024 

## LSH-based similarity join

  * **Project Delivery:** [`Delivery.pdf`](Delivery.pdf)
  * **Project Report:** [`Report.pdf`](Report.pdf)
  * **Project Code:** [`code`](code)
   
## Project Overview

The **LSH-Based Similarity Join** project is designed to perform similarity joins on large datasets using Locality-Sensitive Hashing (LSH) techniques. This approach is essential for efficiently finding pairs of close tuples according to a given distance metric.

### Project Details

- **Course**: SPM (Spatial and Pattern Mining)
- **Academic Year**: 2023/2024
- **Date**: May 17, 2024

### Background

A similarity join operation identifies pairs of tuples from two collections, ΓR and ΓS, based on a distance function `dist(U, V)` and a threshold parameter `λ`. The naive method involves computing the Cartesian product of the two collections, which is computationally expensive.

Locality-Sensitive Hashing (LSH) improves efficiency by hashing tuples into buckets where similar tuples are likely to collide. This hashing technique reduces the number of distance computations needed.

### Project Goals

- Implement a parallel version of the LSH-based similarity join.
- Use **FastFlow** for parallelization on a single multicore node.
- Use **MPI** for distributed computing across a cluster of nodes.
- Develop and validate the solution using both small and large datasets.

## Getting Started

### Prerequisites

- **spmcluster**: The cluster environment where datasets and code are hosted.
- **FastFlow**: For parallelization on multicore nodes.
- **MPI**: For distributed computing across multiple nodes.
- **Makefile/CMake**: For building the project.
- **SLURM**: For job scheduling and running tests.

### Directory Structure

```
/LSH
  ├── src
  │   ├── lshseq.cpp        # Brute-force LSH implementation
  │   ├── parallel_lsh.cpp   # Parallel version using FastFlow and MPI
  ├── datasets
  │   ├── taxi1.dat          # Tiny dataset for quick testing
  │   ├── taxi2.dat          # Tiny dataset for quick testing
  │   ├── lsh1GB.dat         # Large synthetic dataset
  │   ├── lsh5GB.dat         # Larger synthetic dataset
  │   ├── lsh10GB.dat        # Largest synthetic dataset
  ├── Makefile               # Makefile for compiling the source code
  ├── CMakeLists.txt         # CMake configuration
  ├── run_tests.slurm        # SLURM script for running performance tests
  ├── README.md              # This README file
  └── docs
      └── report.pdf         # PDF report with performance analysis and strategy
```

### Building the Project

1. **Clone the Repository**

   ```bash
   git clone https://github.com/yourusername/lsh-based-similarity-join.git
   cd lsh-based-similarity-join
   ```

2. **Compile the Code**

   Using **Makefile**:
   ```bash
   make
   ```

   Using **CMake**:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

### Running the Tests

1. **Prepare the SLURM Job Script**

   Modify the `run_tests.slurm` file as needed to match your cluster environment.

2. **Submit the SLURM Job**

   ```bash
   sbatch run_tests.slurm
   ```

### Datasets

- **Tiny Datasets**: `taxi1.dat`, `taxi2.dat` for quick testing.
- **Large Datasets**: `lsh1GB.dat`, `lsh5GB.dat`, `lsh10GB.dat` for performance evaluation.

### Parameters

- **Threshold (λ)**:
  - **Taxi Datasets**: 0.01
  - **Synthetic Datasets**: 10
- **LSH Resolution**:
  - **Taxi Datasets**: 0.08
  - **Synthetic Datasets**: 80

## Documentation

A comprehensive PDF document (`report.pdf`) is provided in the `docs` directory. It includes:

- Parallelization strategy
- Performance analysis
- Speedup, scalability, and efficiency plots
- Comments and challenges faced

## Contributing

For contributions, please fork the repository and submit a pull request with your changes.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Contact

For questions or issues, please contact [your.email@example.com](mailto:your.email@example.com).

---

Feel free to adjust any specific details as needed!


