# OpenMPI

The Message Passing Interface (MPI) is a standard that defines how processes on different computers can communicate and exchange data. It's a key protocol for building parallel and distributed applications, especially in high-performance computing (HPC) environments. OpenMPI is a popular open-source implementation of the MPI standard.

## Table of Contents

- [Hello World](#hello-world)
- [MPI Reduce](#mpi-reduce)
- [MPI Allreduce](#mpi-allreduce)
- [MPI Scatter](#mpi-scatter)
- [MPI Gather](#mpi-gather)
- [MPI Broadcast](#mpi-broadcast)
- [Numerical Integration using rule - serial](#trapezoidal-rule---serial)
- [Numerical Integration using rule - parallel](#trapezoidal-rule---parallel)
- [Matrix multiplication - serial](#matrix-multiplication---serial)
- [Matrix multiplication - parallel (Row based decomposition)](#matrix-multiplication---parallel-row-based-decomposition)
- [Derived Data Type](#derived-data-type)

---

## Hello World

[01-Helloworld.cpp](../Parallel%20Programming/OpenMPI/01-Helloworld.cpp)

A simple MPI program that prints a "Hello World" message from each process, including its rank. This example demonstrates basic MPI initialization, process identification, and finalization.

**Key MPI functions:**  
`MPI_Init`, `MPI_Comm_rank`, `MPI_Comm_size`, `MPI_Finalize`

---
## MPI Reduce

[Reduce.cpp](../Parallel%20Programming/OpenMPI/reduce.cpp)

The `MPI_Reduce` function performs a reduction operation (like sum, max, min, etc.) across all processes and stores the result only on the **root process**.

**Key MPI function:**  
`MPI_Reduce`

**Use case:**  
Aggregating partial results (e.g., local sums or maxima) computed by each process to a final result on the root process.

---

## MPI Allreduce

[Allreduce.cpp](../Parallel%20Programming/OpenMPI/allreduce.cpp)

The `MPI_Allreduce` function performs a reduction operation across all processes **and distributes the result to every process** in the communicator.

**Key MPI function:**  
`MPI_Allreduce`

**Use case:**  
When all processes need to know the result of a collective reduction (e.g., computing a global average, norm, or total count).

---

## MPI Scatter

[Scatter.cpp](../Parallel%20Programming/OpenMPI/Scatter.cpp)

The `MPI_Scatter` function distributes distinct chunks of data from the root process to all processes in the communicator. Each process receives a portion of the data.

**Key MPI function:**  
`MPI_Scatter`

**Use case:**  
Distributing parts of an array to different processes for parallel computation.

---

## MPI Gather

[Gather.cpp](../Parallel%20Programming/OpenMPI/Gather.cpp)

The `MPI_Gather` function collects data from all processes in a communicator and concatenates them into a buffer on the root process.

**Key MPI function:**  
`MPI_Gather`

**Use case:**  
Collecting computed results from all processes to a single process for aggregation or output.

---

## MPI Broadcast

[Brodcast.cpp](../Parallel%20Programming/OpenMPI/Brodcast.cpp)

The `MPI_Bcast` function broadcasts a message from the root process to all other processes in the communicator.

**Key MPI function:**  
`MPI_Bcast`

**Use case:**  
Sharing common input data or configuration from one process to all others.

---

## Trapezoidal Rule - Serial

[02-trapezoidal_serial.cpp](../Parallel%20Programming/OpenMPI/02-trapezoidal_serial.cpp)

A serial implementation of the trapezoidal rule for numerical integration. This example serves as a baseline for comparing parallel implementations using MPI.

---

## Trapezoidal Rule - Parallel

[02-trapezoidal_parallel.cpp](../Parallel%20Programming/OpenMPI/02-trapezoidal_parallel.cpp)

A parallel implementation of the trapezoidal rule for numerical integration using MPI. This example demonstrates how the integration interval and computation can be distributed among multiple processes.

---

## Matrix multiplication - serial

[03-matrix_multiplication_serial.cpp](../Parallel%20Programming/OpenMPI/03-matrix_multiplication_serial.cpp)

A serial implementation of matrix multiplication. This serves as a reference for understanding the performance benefits of parallel matrix multiplication.

---

## Matrix multiplication - parallel (Row based decomposition)

[03-matrix_multiplication_parallel.cpp](../Parallel%20Programming/OpenMPI/03-matrix_multipliaction_parallel_rowDecom.cpp)

A parallel implementation of matrix multiplication using row-based decomposition with MPI. Each process computes a subset of the result matrix rows.

---

## Derived Data Type

[DerivedDataType.cpp](../Parallel%20Programming/OpenMPI/DerivedDataType.cpp)

Demonstrates the use of custom MPI derived data types to communicate complex data structures efficiently between processes.

**Key MPI functions:**  
`MPI_Type_create_struct`, `MPI_Type_commit`, `MPI_Scatter`

**Use case:**  
Sending and receiving user-defined structures (e.g., structs in C/C++) in a single MPI call.

---

## References

- [OpenMPI Documentation](https://www.open-mpi.org/doc/)
- [MPI Standard](https://www.mpi-forum.org/docs/)
