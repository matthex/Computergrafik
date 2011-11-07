//
// Matrix
//
// Class for simple matrix representations of arbitrary size and type.
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de
// Based on the original work by Gerd Reis.
//

#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

template<class T, unsigned int SIZE> class Vector;

template<class T, unsigned int SIZE>
class Matrix
{
public:
    // Standard constructor, initialize all entries to zero.
    Matrix<T, SIZE>()
    {
        for (unsigned int j = 0; j < SIZE; j++)
            for (unsigned int i = 0; i < SIZE; i++)
                m_data[i][j] = T(0);
    }

    // Destructor
    ~Matrix()
    {
        // Nothing to do
    }

    Matrix<T, SIZE>(const T data[SIZE][SIZE])
    {
        for (unsigned int j = 0; j < SIZE; j++)
            for (unsigned int i = 0; i < SIZE; i++)
                m_data[i][j] = data[i][j];
    }

    // Access operator to modifiy an entry.
    // i: Row, j: Column
    T &operator ()(unsigned int i, unsigned int j)
    {
        if (i >= SIZE)
            i = SIZE-1;
        if (j >= SIZE)
            j = SIZE-1;
        return m_data[i][j];
    }

    // Access operator without modification.
    // i: Row, j: Column
    T operator ()(unsigned int i, unsigned int j) const
    {
        if (i >= SIZE)
            i = SIZE-1;
        if (j >= SIZE)
            j = SIZE-1;
        return m_data[i][j];
    }


    // Matrix<T, SIZE> operator +(const Matrix<T, SIZE> &mat);
    // Matrix<T, SIZE> operator -(const Matrix<T, SIZE> &mat);

    // Left-hand vector-matrix multiplication
    Vector<T, SIZE> operator *(const Vector<T, SIZE> &vec)
    {
        Vector<T, SIZE> buf;
        for (unsigned int j = 0; j < SIZE; j++)
            for (unsigned int i = 0; i < SIZE; i++)
                buf(i) += m_data[i][j] * vec(j);
        return buf;
    }

    // Matrix multiplication
    Matrix<T, SIZE> operator *(const Matrix<T, SIZE> &mat)
    {
        Matrix<T, SIZE> result;
        for(unsigned int i = 0; i < SIZE; i++){
            for(unsigned int j = 0; j < SIZE; j++){
                for(unsigned int k = 0; k < SIZE; k++){
                    result(i,j) = result(i,j) + m_data[i][k] * mat(k,j);
                }
            }
        }
        return result;
    }

    // Compute the inverse of a 4x4 matrix.
    // singular is set to false if the matrix is singular, else true.
    // Based on the work by Burkhard Lehner.
    Matrix<double, 4> inverse(bool &singular)
    {
        Matrix<double, 4> result;
        if (SIZE != 4)
        {
            return result;
        }

        singular = true;

        // Stores the line swaps
        int p[3];
        // Stores the LU decomposition
        Matrix<double, 4> LU = *this;

        // Compute the LU decomposition, using relative maximum pivot element
        for (unsigned int k = 0; k < 3; k++)
        {
            double max = 0.0;
            p[k] = 0;
            for (unsigned int i = k; i < 4; i++)
            {
                double s = 0.0;
                for (unsigned int j = k; j < 4; j++)
                    s += fabs(LU(i,j));
                if (s == 0.0)
                    return result;
                double q = fabs(LU(i,k))/s;
                if (q > max)
                {
                    max = q;
                    p[k] = i;
                }
            }
            if (max == 0.0)
                return result;
            if (p[k] != k)
            {
                for (unsigned int j = 0; j < 4; j++)
                {
                    double temp = LU(k,j);
                    LU(k,j) = LU(p[k],j);
                    LU(p[k],j) = temp;
                }
            }
            for (unsigned int i = k+1; i < 4; i++)
            {
                LU(i,k) /= LU(k,k);
                for (unsigned int j = k+1; j < 4; j++)
                    LU(i,j) -= LU(i,k)*LU(k,j);
            }
        }
        if (LU(3,3) == 0.0)
            return result;

        // Now solve the equation system for the unity vectors
        for (unsigned int k = 0; k < 4; k++)
        {
            // Create the k-th unity vector
            double b[4] = {0.0, 0.0, 0.0, 0.0};
            b[k] = 1.0;

            // Undo the line swaps
            for (unsigned int i = 0; i < 3; i++)
            {
                if (p[i] != i)
                {
                    double temp = b[i];
                    b[i] = b[p[i]];
                    b[p[i]] = temp;
                }
            }

            // Forward substitution
            for (unsigned int i = 0; i < 4; i++)
                for (unsigned int j = 0; j < i; j++)
                    b[i] -= LU(i,j)*b[j];

            // Backward substitution
            for (int i = 3; i >= 0; i--)
            {
                for (int j = i+1; j < 4; j++)
                    b[i] -= LU(i,j)*b[j];
                b[i] /= LU(i,i);
            }

            // Copy the solution into the result matrix
            for (unsigned int i = 0; i < 4; i++)
                result(i,k) = b[i];
        }

        singular = false;
        return result;
    }

private:
    T m_data[SIZE][SIZE];
};

// Some common matrix classes
typedef Matrix<float, 2> Mat2f;
typedef Matrix<float, 3> Mat3f;
typedef Matrix<float, 4> Mat4f;

typedef Matrix<double, 2> Mat2d;
typedef Matrix<double, 3> Mat3d;
typedef Matrix<double, 4> Mat4d;

#endif // MATRIX_H
