import numpy as np
import argparse

def parse_arguments():
    """
    Добавляет аргументы из командной строки
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("file_a", help="Путь к первой матрице (A)")
    parser.add_argument("file_b", help="Путь ко второй матрице (B)")
    parser.add_argument("file_res", help="Путь к результату (R)")
    return parser.parse_args()

def read_matrix(filename):
    """
    Считывает матрицу из файла
    """
    try:
        return np.loadtxt(filename)
    except Exception as e:
        print(f"Ошибка чтения '{filename}': {e}")
        return None

def main():
    args = parse_arguments()
    A = read_matrix(args.file_a)
    B = read_matrix(args.file_b)
    R = read_matrix(args.file_res)
    try:
        result = A @ B
        if np.allclose(result, R):
            print("Результат верен")
        else:
            print("Произошла ошибка")
    except ValueError as e:
        print(f"{e}")

if __name__ == "__main__":
    main()
