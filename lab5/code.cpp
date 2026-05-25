#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;


static inline int idx(int i, int j, int n) {
    return i * n + j;
}


void generate_matrix(vector<int>& m) {
    for (size_t i = 0; i < m.size(); ++i) {
        m[i] = rand() % 100;
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);


    int rank = 0, world_size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);


    srand((unsigned)time(nullptr) + rank * 1234);


    vector<int> test_sizes = {200, 400, 800, 1200, 1600, 2000};


    for (int n : test_sizes) {
        if (n % world_size != 0) {
            if (rank == 0) {
                cout << "Size " << n << " is not divided into " << world_size << " processes\n";
            }
            continue;
        }


        int rows_per_proc = n / world_size;
        int local_ints = rows_per_proc * n;


        vector<int> A;
        vector<int> B(n * n);
        vector<int> A_local(local_ints);
        vector<long long> C_local(local_ints, 0);
        vector<long long> C;


        if (rank == 0) {
            cout << "Processing of matrices of size " << n << "x" << n << endl;
            A.resize(n * n);
            generate_matrix(A);
            generate_matrix(B);
        }


        auto start_time = chrono::high_resolution_clock::now();


        MPI_Bcast(B.data(), n * n, MPI_INT, 0, MPI_COMM_WORLD);


        MPI_Scatter(
            rank == 0 ? A.data() : nullptr,
            local_ints, MPI_INT,
            A_local.data(),
            local_ints, MPI_INT,
            0, MPI_COMM_WORLD
        );


        for (int i = 0; i < rows_per_proc; ++i) {
            for (int j = 0; j < n; ++j) {
                long long sum = 0;
                for (int k = 0; k < n; ++k) {
                    sum += (long long)A_local[i * n + k] * B[k * n + j];
                }
                C_local[i * n + j] = sum;
            }
        }


        if (rank == 0) {
            C.resize(n * n);
        }


        MPI_Gather(
            C_local.data(),
            local_ints, MPI_LONG_LONG,
            rank == 0 ? C.data() : nullptr,
            local_ints, MPI_LONG_LONG,
            0, MPI_COMM_WORLD
        );


        auto end_time = chrono::high_resolution_clock::now();


        if (rank == 0) {
            cout << "Size: " << n
                 << ", Time: " << chrono::duration<double>(end_time - start_time).count()
                 << " seconds\n";
        }


        MPI_Barrier(MPI_COMM_WORLD);
    }


    MPI_Finalize();
    return 0;
}
