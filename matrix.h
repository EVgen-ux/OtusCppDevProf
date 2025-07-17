#pragma once

#include <iostream>
#include <map>
#include <utility>

template<typename T, T defaultValue> class MatrixItemProxy;

template<typename T, T defaultValue>
class Matrix
{
public:
    Matrix() = default;

    ~Matrix() = default;

    size_t size() 
    {
        return m_data.size();
    }

    void print()
    {
        for(const auto &x : m_data)
        {
            std::cout << "[" << x.first.first
                      << ", " << x.first.second << "] = "
                      << x.second << std::endl;
        }
    }

    MatrixItemProxy<T, defaultValue> operator[](size_t row)
    {
        return MatrixItemProxy<T, defaultValue>(*this, row);
    }

private:
    std::map<std::pair<size_t, size_t>, T> m_data;
    friend MatrixItemProxy<T, defaultValue>;
};

template<typename T, T defaultValue>
class MatrixItemProxy
{
public:
    MatrixItemProxy(Matrix<T, defaultValue> &matrix, size_t row) :
        m_row(row),
        m_matrix(matrix)
    {}

    MatrixItemProxy& operator[](size_t col)
    {
        m_col = col;
        return *this;
    } 

    void operator=(T value)
    {
        auto idx = std::make_pair(m_row, m_col);
        if(value == defaultValue)
        {
            auto iter = m_matrix.m_data.find(idx);
            if(iter == m_matrix.m_data.end())
            {
                return;
            }
            m_matrix.m_data.erase(iter);
            return;
        }
        m_matrix.m_data[idx] = value;
    }

    operator T()
    {
        auto idx = std::make_pair(m_row, m_col);
        if(m_matrix.m_data.find(idx) == m_matrix.m_data.end())
        {
            return defaultValue;
        }
        return m_matrix.m_data[idx];
    }

private:
    size_t m_row;
    size_t m_col;
    Matrix<T, defaultValue> &m_matrix;
};
