#include <stdio.h>
#include <stdlib.h>     //for memory allocation
#include <mpi.h>

int main(int argc, char const *argv[]) {
    int m, n, p;                    // dimension of matrices, i.e. A=mxn and B=nxp
    int count;                      // total number of matrices
    int l_count;                    // number of matrices per process
    int size;                       // total number of process
    int id;                         // process rank (0,1,2,.......n-1)
    double t_start, t_end, time;

    int *l_a, *l_b, *l_c;           // loacl matrices for each process

    int *mat_a, *mat_b, *mat_c;     // global matrices

    MPI_Init(NULL, NULL);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (id == 0) {
        scanf("%d", &count);
        scanf("%d %d %d", &m, &n, &p);
        // Memory allocation for global matrices
        mat_a = (int *)malloc(count * m * n * sizeof(int));
        mat_b = (int *)malloc(count * n * p * sizeof(int));
        mat_c = (int *)malloc(count * m * p * sizeof(int));
        // Initialize mat_a and mat_b
        for (int i = 0; i < count * m * n; i++)
            mat_a[i] = 1;

        for (int i = 0; i < count * n * p; i++)
            mat_b[i] = 1;

        // Send matrix information to other processes
        for (int i = 1; i < size; i++) {
            MPI_Send(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&m, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
            MPI_Send(&p, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&m, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&n, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&p, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
    }
    // Each process allocates memory for its local matrices based on the information received.
    l_count = count / size;
    l_a = (int *)malloc(l_count * m * n * sizeof(int));
    l_b = (int *)malloc(l_count * n * p * sizeof(int));
    l_c = (int *)malloc(l_count * m * p * sizeof(int));

    // The global matrices (mat_a and mat_b) are scattered to local matrices (l_a and l_b) among processes.
    MPI_Scatter(mat_a, l_count * m * n, MPI_INT, l_a, l_count * m * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(mat_b, l_count * n * p, MPI_INT, l_b, l_count * m * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
   
    // Arrays l_a and l_b are converted to 3D matrices (m_a and m_b) for easier computation.
    int m_a[l_count][m][n];
    int m_b[l_count][n][p];
    int m_c[l_count][m][p];

    int idx = 0;
    for (int c = 0; c < l_count; c++)
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                m_a[c][i][j] = l_a[idx++];

    idx = 0;
    for (int c = 0; c < l_count; c++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < p; j++)
                m_b[c][i][j] = l_b[idx++];

    // The matrix multiplication is performed using nested loops. Timing information is also recorded.
    t_start = MPI_Wtime();
    for (int c = 0; c < l_count; c++) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                m_c[c][i][j] = 0;
                for (int k = 0; k < p; k++) {
                    m_c[c][i][j] += m_a[c][i][k] * m_b[c][k][j];
                }
            }
        }
    }
    t_end = MPI_Wtime();
    time = t_end - t_start;
    printf("PID-%d: Total Time: %f\n", id, time);

    // Local matrices (l_c) are gathered to the global matrix (mat_c) among processes.
    idx = 0;
    for (int c = 0; c < l_count; c++) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                l_c[idx++] = m_c[c][i][j];
            }
        }
    }

    MPI_Gather(l_c, l_count * m * p, MPI_INT, mat_c, l_count * m * p, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // if (id == 0) {
    //     printf("Resulting Matrix (mat_c):\n");
    //    /* for (int c = 0; c < count; c++) {
    //         printf("Matrix %d:\n", c);
    //         for (int i = 0; i < m; i++) {
    //             for (int j = 0; j < p; j++) {
    //                 printf("%d ", mat_c[(c * m * p) + (i * p) + j]);
    //             }
    //             printf("\n");
    //         }
    //         printf("\n");
    //     }*/
    // }

    MPI_Finalize();
    // release memory allocated for the local result matrix (l_c).
    free(l_a);
    free(l_b);
    free(l_c);
    // the memory allocated for global matrices (mat_a, mat_b, mat_c) is only freed by the process with rank 0
    if (id == 0) {
        free(mat_a);
        free(mat_b);
        free(mat_c);
    }
    return 0;
}
