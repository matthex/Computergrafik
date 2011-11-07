//
// Vector
//
// Class for vector data of arbitrary size and type.
// Some functionality is only available for certain configurations (e.g. cross product).
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de
// Based on the original work by Gerd Reis.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <math.h>
#include "matrix.h"

template<class T, unsigned int SIZE> class Matrix;

template<class T, unsigned int SIZE>
class Vector
{
public:
    // Standard constructor
    Vector<T, SIZE>()
    {
        // Initialize all elements with zero
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = T(0);
    }

    // Constructor with data array
    Vector<T, SIZE>(const T data[SIZE])
    {
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = data[i];
    }

    // Convenience constructor for 3D data
    Vector<T, SIZE>(T a, T b, T c)
    {
        if (SIZE == 3)
        {
            m_data[0] = a;
            m_data[1] = b;
            m_data[2] = c;
        }
    }

    // Convenience constructor for 4D data
    Vector<T, SIZE>(T a, T b, T c, T d)
    {
        if (SIZE == 4)
        {
            m_data[0] = a;
            m_data[1] = b;
            m_data[2] = c;
            m_data[3] = d;
        }
    }

    // Copy constructor
    Vector<T, SIZE>(const Vector<T, SIZE> &vec)
    {
        if (this == &vec)
            return; // nothing to do

        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = vec.m_data[i];
    }

    // Destructor
    ~Vector()
    {
        // Nothing to do
    }

    void setData(const T data[SIZE])
    {
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = data[i];
    }

    void getData(T data[SIZE])
    {
        for (unsigned int i = 0; i < SIZE; i++)
            data[i] = m_data[i];
    }

    unsigned int getDimension()
    {
        return SIZE;
    }

    // Assignment operator
    Vector<T, SIZE> &operator =(const Vector<T, SIZE> &vec)
    {
        if (this == &vec)
            return (*this);

        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = vec.m_data[i];

        return (*this);
    }

    // Overloaded assignment operator
    Vector<T, SIZE> &operator =(const T data[SIZE])
    {
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] = data[i];

        return (*this);
    }

    // Usage of operator:
    // vec(i) = var; // 0 <= i <= SIZE-1
    // var = vec(i);
    // vec1(i) = vec2(j);
    T &operator ()(unsigned int i)
    {
        if (i >= SIZE)
            i = SIZE-1; // Operator clips index!
        return m_data[i];
    }

    T operator ()(unsigned int i) const
    {
        if (i >= SIZE)
            i = SIZE-1; // Operator clips index!
        return m_data[i];
    }

    void operator += (const Vector<T, SIZE> &vec)
    {
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] += vec.m_data[i];
    }

    Vector<T, SIZE> operator +(const Vector<T, SIZE> &vec)
    {
        Vector<T, SIZE> buf(m_data);
        for (unsigned int i = 0; i < SIZE; i++)
            buf.m_data[i] += vec.m_data[i];
        return buf;
    }

    void operator -=(const Vector<T, SIZE> &vec)
    {
        for (unsigned int i = 0; i < SIZE; i++)
            m_data[i] -= vec.m_data;
    }

    Vector<T, SIZE> operator -(const Vector<T, SIZE> &vec)
    {
        Vector<T, SIZE> buf(m_data);
        for (unsigned int i = 0; i < SIZE; i++)
            buf.m_data[i] -= vec.m_data[i];
        return buf;
    }

    // Cross product: Only defined for Vec3+Hom
    // Homogeneous coordinate is ignored and set to 1.
    Vector<T, 4> crossH(const Vector<T, 4> &vec)
    {
        T buf[4];
        buf[0]  = m_data[1] * vec.m_data[2];
        buf[0] -= m_data[2] * vec.m_data[1];
        buf[1]  = m_data[2] * vec.m_data[0];
        buf[1] -= m_data[0] * vec.m_data[2];
        buf[2]  = m_data[0] * vec.m_data[1];
        buf[2] -= m_data[1] * vec.m_data[0];
        buf[3] = 1;
        return Vector<T, 4>(buf);
    }

    // Cross product: Only defined for Vec3
    Vector<T, 3> cross(const Vector<T, 3> &vec)
    {
        T buf[3];
        buf[0]  = m_data[1] * vec.m_data[2];
        buf[0] -= m_data[2] * vec.m_data[1];
        buf[1]  = m_data[2] * vec.m_data[0];
        buf[1] -= m_data[0] * vec.m_data[2];
        buf[2]  = m_data[0] * vec.m_data[1];
        buf[2] -= m_data[1] * vec.m_data[0];
        return Vector<T, 3>(buf);
    }

    // Scalar product of two vectors with same dimension
    double dot(const Vector<T, SIZE> &vec)
    {
        double ret = 0;
        for (unsigned int i = 0; i < SIZE; i++)
            ret += m_data[i] * vec.m_data[i];
        return ret;
    }

    // Norm is only defined for Vec3+Hom.
    // Homogeneous coordinate is normalized to 1,
    // then the other coordinates are normalized to 1 as Vec3.
    const Vector<T, 4> normH()
    {
        int i;
        T buf[4];
        T d = 0;
        if (m_data[3] != 1)
        {
            for (i = 0; i < 3; i++)
                buf[i] = m_data[i] / m_data[3];
        }
        else
        {
            for (i = 0; i < 3; i++)
                buf[i] = m_data[i];
        }
        buf[3] = 1;

        for (i = 0; i < 3; i++)
            d += buf[i]*buf[i];
        d = sqrt(d);

        for (i = 0; i < 3; i++)
            buf[i] = buf[i] / d;

        return Vector<T, 4>(buf);
    }

    // Normalizes the length of a vector to 1.
    const Vector<T, SIZE> norm()
    {
        T buf[SIZE];
        double d = 0.0;
        for (unsigned int i = 0; i < SIZE; i++)
            d += m_data[i] * m_data[i];
        d = sqrt(d);

        for (unsigned int i = 0; i < SIZE; i++)
            buf[i] = static_cast<T>(static_cast<double>(m_data[i]) / d);

        return Vector<T, SIZE>(buf);
    }

    double length()
    {
        double d = 0.0;
        for (unsigned int i = 0; i < SIZE; i++)
            d += m_data[i] * m_data[i];
        return sqrt(d);
    }

    // Unary operator, switches sign of each entry.
    Vector<T, SIZE> operator -()
    {
        T buf[SIZE];
        for (unsigned int i = 0; i < SIZE; i++)
            buf[i]  = -m_data[i];
        return Vector<T, SIZE>(buf);
    }

    // Dot product of two vectors of the same size and type.
    T operator *(const Vector<T, SIZE> &vec)
    {
        T dp = T(0);
        for (unsigned int i = 0; i < SIZE; i++)
            dp += m_data[i]*vec.m_data[i];
        return dp;
    }

    // Scalar multiplication.
    // Usage:
    // Vec<double, 3> vec1;
    // double s = 5.0;
    // Vec<double, 3> vec2 = vec1*s;
    Vector<T, SIZE> operator *(T scale)
    {
        T buf[SIZE];
        for (unsigned int i = 0; i < SIZE; i++)
            buf[i] = m_data[i]*scale;
        return Vector<T, SIZE>(buf);
    }

    // Right-hand matrix multiplication.
    Vector<T, SIZE> operator *(const Matrix<T, SIZE> &mat)
    {
        Vector<T, SIZE> vec;
        for (unsigned int j = 0; j < SIZE; j++)
            for (unsigned int i = 0; i < SIZE; i++)
                vec(j) += m_data[i]*mat(i,j);
        return vec;
    }

private:
    T m_data[SIZE];
};

// Some common vector classes
typedef Vector<float, 2> Vec2f;
typedef Vector<float, 3> Vec3f;
typedef Vector<float, 4> Vec4f;

typedef Vector<double, 2> Vec2d;
typedef Vector<double, 3> Vec3d;
typedef Vector<double, 4> Vec4d;

typedef Vector<int, 2> Vec2i;
typedef Vector<int, 3> Vec3i;
typedef Vector<int, 4> Vec4i;

typedef Vector<bool, 2> Vec2b;
typedef Vector<bool, 3> Vec3b;
typedef Vector<bool, 4> Vec4b;

#endif // VECTOR_H
