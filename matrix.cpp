#include "matrix.h"

int main()
{
    Matrix<int, 0> matrix;
    constexpr size_t diagonalLength = 10;

    for(size_t i = 0; i < diagonalLength; ++i)
    {
        matrix[i][i] = i;
        matrix[i][diagonalLength - i - 1] = diagonalLength - i - 1;
    }

    for(size_t i = 1; i < 9; ++i)
    {
        for(size_t j = 1; j < 9; ++j)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "Matrix size: " << matrix.size() << std::endl;
    
    matrix.print();

    return 0;
}