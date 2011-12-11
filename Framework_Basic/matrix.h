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

    //Rotation matrix in 3D: Z axis
    Matrix<double, 4> makeRotMatZ(double angle)
    {
        Matrix<double, 4> rotMat;
        rotMat(0,0) = cos(angle);
        rotMat(0,1) = -sin(angle);
        rotMat(1,0) = sin(angle);
        rotMat(1,1) = cos(angle);
        rotMat(2,2) = 1;
        rotMat(3,3) = 1;        //Other entries are 0

        return rotMat;
    }

    //Rotation matrix in 3D: X axis
    Matrix<double, 4> makeRotMatX(double angle)
    {
        Matrix<double, 4> rotMat;
        rotMat(1,1) = cos(angle);
        rotMat(1,2) = -sin(angle);
        rotMat(2,1) = sin(angle);
        rotMat(2,2) = cos(angle);
        rotMat(0,0) = 1;
        rotMat(3,3) = 1;        //Other entries are 0

        return rotMat;
    }

    //Rotation matrix in 3D: Y axis
    Matrix<double, 4> makeRotMatY(double angle)
    {
        Matrix<double, 4> rotMat;
        rotMat(0,0) = cos(angle);
        rotMat(0,2) = sin(angle);
        rotMat(2,0) = -sin(angle);
        rotMat(2,2) = cos(angle);
        rotMat(1,1) = 1;
        rotMat(3,3) = 1;        //Other entries are 0

        return rotMat;
    }

    //Translation matrix in 3D
    Matrix<double, 4> makeTransMat(Vector<T, SIZE> vec)
    {
        Matrix<double, 4> transMat;
        transMat(0,0) = 1;
        transMat(1,1) = 1;
        transMat(2,2) = 1;
        transMat(3,3) = 1;
        transMat(0,3) = vec(0);
        transMat(1,3) = vec(1);
        transMat(2,3) = vec(2);     //Other entries are 0

        return transMat;
    }

    //Rotation matrix in 3D: any axis
    Matrix<double, 4> makeRotMat(double angle, Vector<T, SIZE> &vec1)
    {
        //Vector of length 1
        double vecLength = sqrt(vec1(0)*vec1(0) + vec1(1)*vec1(1) + vec1(2)*vec1(2));

        Vector<T, SIZE> vec = Vector<T, SIZE>(vec1(0)/vecLength,vec1(1)/vecLength,vec1(2)/vecLength,1);

        Matrix<double, 4> rotMat;
        double d = sqrt(vec(0)*vec(0) + vec(1)*vec(1));

        //Rz
        Matrix<double, 4> rotZ;
        rotZ(0,0) = vec(0)/d;
        rotZ(0,1) = vec(1)/d;
        rotZ(1,0) = -vec(1)/d;
        rotZ(1,1) = vec(0)/d;
        rotZ(2,2) = 1;
        rotZ(3,3) = 1;

        //Ry
        Matrix<double, 4> rotY;
        rotY(0,0) = vec(2);
        rotY(0,2) = -d;
        rotY(2,0) = d;
        rotY(1,1) = 1;
        rotY(2,2) = vec(2);
        rotY(3,3) = 1;

        //Rz2
        Matrix<double, 4> rotZ2;
        rotZ2 = makeRotMatZ(angle);
        if (d==0) return rotZ2; // Rotation um z-Achse

        //Rz return
        Matrix<double, 4> rotZre;
        rotZre(0,0) = vec(0)/d;
        rotZre(0,1) = -vec(1)/d;
        rotZre(1,0) = vec(1)/d;
        rotZre(1,1) = vec(0)/d;
        rotZre(2,2) = 1;
        rotZre(3,3) = 1;

        //Ry return
        Matrix<double, 4> rotYre;
        rotYre(0,0) = vec(2);
        rotYre(0,2) = d;
        rotYre(2,0) = -d;
        rotYre(1,1) = 1;
        rotYre(2,2) = vec(2);
        rotYre(3,3) = 1;

        //Rotation matrix
        rotMat = rotZ*rotY*rotZ2*rotYre*rotZre;

        return rotMat;
    }

    //Rotation matrix in 3D: any axis, any point
    Matrix<double, 4> makeRotMatPoint(double angle, Vector<T, SIZE> &vec, Vector<T, SIZE> p_vec)
    {
        return makeTransMat(p_vec)*makeRotMat(angle,vec)*makeTransMat(-p_vec);
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
