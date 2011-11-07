//
// Point2D
//
// Simple 2D integer point representation.
//
// Author: Dr. Gerd Reis

#ifndef POINT2D_H
#define POINT2D_H

class Point2D
{
public:
    Point2D(int x = 0, int y = 0)
    {
        this->x = x;
        this->y = y;
    }

    int x,y;
};

#endif // POINT2D_H
