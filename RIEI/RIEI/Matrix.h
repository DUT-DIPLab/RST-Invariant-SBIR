#pragma once

#include <iostream>

template<typename T>
class Matrix
{
public:
    Matrix(int row = 0, int col = 0) : _row(0), _col(0), _data(nullptr)
    {
        init(row, col);
    }

    virtual ~Matrix()
    {
        clear();
    }

    void init(int row, int col)
    {
        clear();
        _row = row;
        _col = col;
        _data = new T*[_row];
        for (int i = 0; i < _row; ++i)
        {
            _data[i] = new T[_col];
        }
    }

    inline int row() const
    {
        return _row;
    }

    inline int col() const
    {
        return _col;
    }

    inline T* operator[](int index)
    {
        return _data[index];
    }

    inline const T* operator[](int index) const
    {
        return _data[index];
    }

    inline T safeGet(int row, int col)
    {
        if (0 <= row && row < _row)
        {
            if (0 <= col && col < _col)
            {
                return _data[row][col];
            }
        }
        return 0.0;
    }

    int getPositiveNum() const
    {
        int num = 0;
        for (int i = 0; i < row(); ++i)
        {
            for (int j = 0; j < col(); ++j)
            {
                num += _data[i][j] > 0;
            }
        }
        return num;
    }

    Matrix<T> crop(int top, int left, int height, int width)
    {
        Matrix<T> ret(height, width);
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width; ++j)
            {
                ret[i][j] = _data[i + top][j + left];
            }
        }
        return ret;
    }

    friend std::ostream& operator<<(std::ostream& out, Matrix<T>& mat)
    {
        for (int i = 0; i < mat.row(); ++i)
        {
            for (int j = 0; j < mat.col(); ++j)
            {
                out << mat[i][j] << ' ';
            }
            out << endl;
        }
        return out;
    }

protected:
    int _row, _col;
    T** _data;

    void clear()
    {
        if (_data != nullptr)
        {
            for (int i = 0; i < _row; ++i)
            {
                delete[] _data[i];
            }
            delete[] _data;
            _data = nullptr;
            _row = _col = 0;
        }
    }
};
