#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h>

using namespace std;

static inline int idx(int i, int j, int n) {
    return i * n + j;
}

void generate_matrix(const string& filename, int size) {
    ofstream ofs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ofs << (rand() % 100) << ' ';
        }
        ofs << '\n';
    }
}

vector<vector<int>> read_matrix(const string& filename, int size) {
    vector<vector<int>> matrix(size, vector<int>(size));
    ifstream ifs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ifs >> matrix[i][j];
        }
    }
    return matrix;
}

void save_result(const string& filename, const vector<vector<long long>>& matrix, int size) {
    ofstream ofs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ofs << matrix[i][j] << ' ';
        }
        ofs << '\n';
    }
}

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "");

    MPI_Init(&argc, &argv);

    int rank = 0, world_size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    vector<int> sizes = {100, 200, 400, 800, 1200, 1600, 2000};
    srand((unsigned)time(nullptr) + rank * 1337);

    for (int size : sizes) {
        if (size % world_size != 0) {
            if (rank == 0) {
                cout << "Size " << size << " is not divisible by " << world_size << " processes\n";
            }
            continue;
        }

        int rows_per_proc = size / world_size;
        int local_ints = rows_per_proc * size;

        string fileA = "matrixA_" + to_string(size) + ".txt";
        string fileB = "matrixB_" + to_string(size) + ".txt";
        string fileC = "result_" + to_string(size) + ".txt";

        vector<int> A;
        vector<int> B(size * size);
        vector<int> A_local(local_ints);
        vector<long long> C_local(local_ints, 0);
        vector<long long> C;

        if (rank == 0) {
            cout << "Processing matrices of size " << size << "x" << size << endl;

            generate_matrix(fileA, size);
            generate_matrix(fileB, size);

            auto A2d = read_matrix(fileA, size);
            auto B2d = read_matrix(fileB, size);

            A.resize(size * size);
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    A[idx(i, j, size)] = A2d[i][j];
                    B[idx(i, j, size)] = B2d[i][j];
                }
            }
        }

        MPI_Bcast(B.data(), size * size, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Scatter(
            rank == 0 ? A.data() : nullptr,
            local_ints, MPI_INT,
            A_local.data(),
            local_ints, MPI_INT,
            0, MPI_COMM_WORLD
        );

        auto start = chrono::high_resolution_clock::now();

        for (int i = 0; i < rows_per_proc; ++i) {
            for (int j = 0; j < size; ++j) {
                long long sum = 0;
                for (int k = 0; k < size; ++k) {
                    sum += (long long)A_local[i * size + k] * B[k * size + j];
                }
                C_local[i * size + j] = sum;
            }
        }

        if (rank == 0) {
            C.resize(size * size);
        }

        MPI_Gather(
            C_local.data(),
            local_ints, MPI_LONG_LONG,
            rank == 0 ? C.data() : nullptr,
            local_ints, MPI_LONG_LONG,
            0, MPI_COMM_WORLD
        );

        auto end = chrono::high_resolution_clock::now();

        if (rank == 0) {
            vector<vector<long long>> C2d(size, vector<long long>(size));
            for (int i = 0; i < size; ++i) {
                for (int j = 0; j < size; ++j) {
                    C2d[i][j] = C[idx(i, j, size)];
                }
            }

            save_result(fileC, C2d, size);

            cout << "Multiplication time: "
                 << chrono::duration<double>(end - start).count()
                 << " seconds\n";
            cout << "Result saved to file: " << fileC << "\n\n";
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
