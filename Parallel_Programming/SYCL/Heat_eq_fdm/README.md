# Instructions

The program is compiled using the oneAPI `icpx` compiler.

Follow the commands below to compile and run the program:

```sh
icpx -fsycl main.cpp -o Diffusion
./Diffusion number_of_rows number_of_columns number_of_time_iterations
```

- `number_of_rows`: Number of rows in the grid
- `number_of_columns`: Number of columns in the grid
- `number_of_time_iterations`: Number of time steps to simulate

**Example:**

```sh
./Diffusion 256 256 1500
```