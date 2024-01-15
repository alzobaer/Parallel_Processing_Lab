# Parallel Processing Lab

This repository contains programs developed for the Parallel Processing Lab. The lab includes implementations in MPI (Message Passing Interface) and CUDA (Compute Unified Device Architecture) for various parallel computing tasks.

## Task 1: Matrix Multiplication

### MPI Implementation (`task1_mpi_matrix_multiply.c`)
- The program multiplies K different matrices A of dimension MxN with matrices B of dimension NxP.
- Constraints: K * M * N <= 10^6; K * N * P <= 10^6; K * M * P <= 10^6;

#### Input:
- K, M, N, P

#### Output:
- Time taken for multiplication

### CUDA Implementation (`task1_cuda_matrix_multiply.cu`)
- The CUDA version of the program performs matrix multiplication using parallel GPU processing.

#### Input:
- K, M, N, P

#### Output:
- Time taken for multiplication

## Task 2: Word Count and Sorting

### MPI Implementation (`task2_mpi_word_count.c`)
- The program counts the words in a file and sorts them in descending order of frequency using MPI.

#### Input:
- No. of processes, Text input from file

#### Output:
- Total time taken
- Top 10 occurrences

### CUDA Implementation (`task2_cuda_word_count.cu`)
- The CUDA version counts words and performs sorting for improved parallelism on GPU.

#### Input:
- No. of processes, Text input from file

#### Output:
- Total time taken
- Top 10 occurrences

## Task 3: Phonebook Search

### MPI Implementation (`task3_mpi_phonebook_search.c`)
- The program searches for contacts in a phonebook file matching a given name using MPI.

#### Input:
- No. of processes, Phonebook from file

#### Output:
- Total time taken
- Matching names and contact numbers

### CUDA Implementation (`task3_cuda_phonebook_search.cu`)
- The CUDA version accelerates the phonebook search on the GPU for improved parallel performance.

#### Input:
- No. of processes, Phonebook from file

#### Output:
- Total time taken
- Matching names and contact numbers

## Task 4: Pattern Occurrence Count

### MPI Implementation (`task4_mpi_pattern_occurrence.c`)
- The program finds the number of occurrences of a given pattern in a paragraph using MPI.

#### Input:
- No. of processes, Paragraph from file

#### Output:
- Total time taken
- No. of occurrences of the pattern

### CUDA Implementation (`task4_cuda_pattern_occurrence.cu`)
- The CUDA version efficiently calculates pattern occurrences using GPU parallelism.

#### Input:
- No. of processes, Paragraph from file

#### Output:
- Total time taken
- No. of occurrences of the pattern

Feel free to explore each task's respective source code for detailed comments and understand the parallel processing implementations.
