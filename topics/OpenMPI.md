# OpenMPI

The Message Passing Interface (MPI) is a standard that defines how processes on different computers can communicate and exchange data. It's a key protocol for building parallel and distributed applications, especially in high-performance computing (HPC) environments. OpenMPI is a popular open-source implementation of the MPI standard.

## Table of Contents

- [Hello World](#hello-world)
- [MPI Scatter](#mpi-scatter)
- [MPI Gather](#mpi-gather)
- [MPI Broadcast](#mpi-broadcast)
- [Trapezoidal Rule - Serial](#trapezoidal-rule---serial)
- [Derived Data Type](#derived-data-type)

---

## Hello World

[01-Helloworld.cpp](OpenMPI/01-Helloworld.cpp)

A simple MPI program that prints a "Hello World" message from each process, including its rank and the total number of processes. This example demonstrates basic MPI initialization, process identification, and finalization.

**Key MPI functions:**  
`MPI_Init`, `MPI_Comm_rank`, `MPI_Comm_size`, `MPI_Finalize`

---

## MPI Scatter

[Scatter.cpp](OpenMPI/Scatter.cpp)

The `MPI_Scatter` function distributes distinct chunks of data from the root process to all processes in the communicator. Each process receives a portion of the data.

**Key MPI function:**  
`MPI_Scatter`

**Use case:**  
Distributing parts of an array to different processes for parallel computation.

---

## MPI Gather

[Gather.cpp](OpenMPI/Gather.cpp)

The `MPI_Gather` function collects data from all processes in a communicator and concatenates them into a buffer on the root process.

**Key MPI function:**  
`MPI_Gather`

**Use case:**  
Collecting computed results from all processes to a single process for aggregation or output.

---

## MPI Broadcast

[Brodcast.cpp](OpenMPI/Brodcast.cpp)

The `MPI_Bcast` function broadcasts a message from the root process to all other processes in the communicator.

**Key MPI function:**  
`MPI_Bcast`

**Use case:**  
Sharing common input data or configuration from one process to all others.

---

## Trapezoidal Rule - Serial

[02-trapezoidal_serial](OpenMPI/02-trapezoidal_serial)

A serial implementation of the trapezoidal rule for numerical integration. This example serves as a baseline for comparing parallel implementations using MPI.

---

## Derived Data Type

[Scatter.cpp](OpenMPI/Scatter.cpp)

Demonstrates the use of custom MPI derived data types to communicate complex data structures efficiently between processes.

**Key MPI functions:**  
`MPI_Type_create_struct`, `MPI_Type_commit`, `MPI_Scatter`

**Use case:**  
Sending and receiving user-defined structures (e.g., structs in C/C++) in a single MPI call.

---

## References

- [OpenMPI Documentation](https://www.open-mpi.org/doc/)
- [MPI Standard](https://www.mpi-forum.org/docs/)
