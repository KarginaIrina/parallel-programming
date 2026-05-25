#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cuda_runtime.h>

using namespace std;

// CUDA ядро для умножения матриц
__global__ void matrixMultiplyKernel(const int* A, const int* B, long long* C, int N) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < N && col < N) {
        long long sum = 0;
        for (int k = 0; k < N; ++k) {
            sum += (long long)A[row * N + k] * B[k * N + col];
        }
        C[row * N + col] = sum;
    }
}

// Генерация матрицы
void generate_matrix(vector<int>& matrix, int size) {
    for (int i = 0; i < size * size; ++i) {
        matrix[i] = rand() % 100;
    }
}

// Основная функция
int main() {
    srand(time(nullptr));

    vector<int> sizes = {100, 200, 400, 800, 1200, 1600, 2000};

    for (int size : sizes) {
        cout << "Обработка матриц размера " << size << "x" << size << endl;

        size_t matrix_bytes = size * size * sizeof(int);
        size_t result_bytes = size * size * sizeof(long long);

        // Векторы для матриц
        vector<int> h_A(size * size);
        vector<int> h_B(size * size);
        vector<long long> h_C(size * size);

        generate_matrix(h_A, size);
        generate_matrix(h_B, size);

        // Указатели на GPU
        int *d_A, *d_B;
        long long *d_C;

        cudaMalloc(&d_A, matrix_bytes);
        cudaMalloc(&d_B, matrix_bytes);
        cudaMalloc(&d_C, result_bytes);

        cudaMemcpy(d_A, h_A.data(), matrix_bytes, cudaMemcpyHostToDevice);
        cudaMemcpy(d_B, h_B.data(), matrix_bytes, cudaMemcpyHostToDevice);

        // Настройка конфигурации блоков и сетки
        dim3 blockDim(16, 16);
        dim3 gridDim((size + blockDim.x - 1) / blockDim.x,
                     (size + blockDim.y - 1) / blockDim.y);

        auto start = chrono::high_resolution_clock::now();

        // Запуск ядра
        matrixMultiplyKernel<<<gridDim, blockDim>>>(d_A, d_B, d_C, size);
        cudaDeviceSynchronize();

        auto end = chrono::high_resolution_clock::now();

        // Копирование результата
        cudaMemcpy(h_C.data(), d_C, result_bytes, cudaMemcpyDeviceToHost);

        chrono::duration<double> diff = end - start;

        cout << "Время умножения на GPU: " << diff.count() << " секунд" << endl;
        cout << "Объем задач: " << 2LL * size * size * size << " операций" << endl;

        // Очистка
        cudaFree(d_A);
        cudaFree(d_B);
        cudaFree(d_C);

        cout << "----------------------------------------" << endl;
    }

    return 0;
}
