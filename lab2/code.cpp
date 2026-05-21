#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>
#include <chrono>
#include <Windows.h>
#include <omp.h>

using namespace std;


// Функция генерации матрицы и сохранения в файл
void generate_matrix(const string& filename, int size) {
    ofstream ofs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int val = rand() % 100;
            ofs << val << " ";
        }
        ofs << "\n";
    }
    ofs.close();
}

// Функция чтения матрицы из файла
vector<vector<int>> read_matrix(const string& filename, int size) {
    vector<vector<int>> matrix(size, vector<int>(size));
    ifstream ifs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ifs >> matrix[i][j];
        }
    }
    ifs.close();
    return matrix;
}

// Функция записи матрицы результата в файл
void save_result(const string& filename, const vector<vector<long long>>& matrix, int size) {
    ofstream ofs(filename);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            ofs << matrix[i][j] << " ";
        }
        ofs << "\n";
    }
    ofs.close();
}

// Модифицированная функция умножения матриц с OpenMP
vector<vector<long long>> multiply_matrices_parallel(const vector<vector<int>>& A, const vector<vector<int>>& B, int size, int num_threads) {
    vector<vector<long long>> C(size, vector<long long>(size, 0));

    omp_set_num_threads(num_threads);

    #pragma omp parallel for
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            long long sum = 0;
            for (int k = 0; k < size; ++k) {
                sum += (long long)A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
    return C;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    vector<int> thread_counts = {1, 2, 4, 8}; 

    for (int size : sizes) {
        cout << "Обработка матриц размера " << size << "x" << size << endl;

        string fileA = "matrixA_" + to_string(size) + ".txt";
        string fileB = "matrixB_" + to_string(size) + ".txt";
        string fileC = "result_" + to_string(size) + ".txt";

        srand(time(nullptr));

        generate_matrix(fileA, size);
        generate_matrix(fileB, size);

        auto A = read_matrix(fileA, size);
        auto B = read_matrix(fileB, size);

        for (int thread_num : thread_counts) {
            auto start = chrono::high_resolution_clock::now();

            auto C = multiply_matrices_parallel(A, B, size, thread_num);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> diff = end - start;

            save_result(fileC, C, size);

            cout << "Потоков: " << thread_num << "\tВремя: " << diff.count() << " секунд" << endl;
        }

        cout << endl;
    }

    return 0;
}
