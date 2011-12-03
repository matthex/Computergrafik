#ifndef CLOCK_H
#define CLOCK_H

#include "vector.h"
#include "matrix.h"

class Clock
{
public:
    // Standard constructor
    Clock();

    //2nd contructor
    Clock(int winWidth, int winHeight, Vec3d center, double radius, Vec3d transVec);

    // Destructor
    ~Clock();

    Vec3d getCenter();

    double getRadius();

    Vec3d getLonghand();

    Vec3d getShorthand();

    void update(int elapsed);

    Mat3d makeRotMat(double angle);

    Mat3d makeTransMat(Vec3d transVec);

private:
    Vec3d m_center;         //Center of clock
    double m_radius;
    Vec3d m_longhand;       //End of long hand
    Vec3d m_shorthand;      //End of short hand
    double m_longhandAngle;
    double m_shorthandAngle;
    Mat3d m_longhandRot;
    Mat3d m_shorthandRot;
    Vec3d m_transVec;
    Mat3d m_transMat;
    int m_longhandCounter;
    int m_winWidth;
    int m_winHeight;
};

#endif // CLOCK_H
