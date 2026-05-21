#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib> 
#include <ctime>
#include <chrono>
#include <Windows.h>

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

// Функция умножения матриц
vector<vector<long long>> multiply_matrices(const vector<vector<int>>& A, const vector<vector<int>>& B, int size) {
    vector<vector<long long>> C(size, vector<long long>(size, 0));
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

int main() {
    SetConsoleOutputCP(CP_UTF8);
    vector<int> sizes = {100, 200, 400, 800};

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

        auto start = chrono::high_resolution_clock::now();

        auto C = multiply_matrices(A, B, size);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> diff = end - start;

        save_result(fileC, C, size);

        double time_sec = diff.count();
        long long operations = 2LL * size * size * size;
        cout << "Время умножения: " << time_sec << " секунд" << endl;
        cout << "Объем задачи (операций): " << operations << endl;
        cout << "Результат сохранен в файл: " << fileC << "\n" << endl;
    }

    return 0;
}
