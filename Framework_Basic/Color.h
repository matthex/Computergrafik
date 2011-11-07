//
// Color
//
// Simple color representation as a 3-tuple.
//
// Author: Dr. Gerd Reis
//

#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
    Color(double r = 1.0, double g = 1.0, double b = 1.0)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    double r,g,b;
};

#endif // COLOR_H
