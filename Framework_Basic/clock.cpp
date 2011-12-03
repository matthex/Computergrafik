#include "clock.h"
#include <QDebug>

Clock::Clock()
{

}

Clock::Clock(int winWidth, int winHeight, Vec3d center, double radius, Vec3d transVec)
{
    m_center = center;
    m_radius = radius;
    m_longhand = Vec3d(m_center(0), m_center(1)+m_radius*0.9, m_center(2));
    m_shorthand = Vec3d(m_center(0), m_center(1)+m_radius*0.6, m_center(2));

    m_longhandAngle = 0.048;
    m_shorthandAngle = 0.004;

    m_transVec = transVec;
    m_transMat = makeTransMat(m_transVec);

    m_winWidth = winWidth;
    m_winHeight = winHeight;
}

Clock::~Clock()
{
    //Nothing to do
}

Vec3d Clock::getCenter()
{
    return m_center;
}

double Clock::getRadius()
{
    return m_radius;
}

Vec3d Clock::getLonghand()
{
    return m_longhand;
}

Vec3d Clock::getShorthand()
{
    return m_shorthand;
}

void Clock::update(int elapsed)
{
    //Rotate hands
    Mat3d mat1;
    mat1(0,0) = 1;
    mat1(1,1) = 1;
    mat1(2,2) = 1;
    mat1(0,2) = m_center(0);
    mat1(1,2) = m_center(1);

    Mat3d mat2;
    mat2(0,0) = 1;
    mat2(1,1) = 1;
    mat2(2,2) = 1;
    mat2(0,2) = -m_center(0);
    mat2(1,2) = -m_center(1);

    m_longhand = mat1*makeRotMat(-m_longhandAngle)*mat2*m_longhand;
    m_shorthand = mat1*makeRotMat(-m_shorthandAngle)*mat2*m_shorthand;

    //Move clock
    m_center = m_transMat*m_center;
    m_longhand = m_transMat*m_longhand;
    m_shorthand = m_transMat*m_shorthand;

    //Check collission
    if(m_center(0)+m_radius+abs(m_transVec(0))+2 > m_winWidth || m_center(0)-m_radius-abs(m_transVec(0))-2 < -m_winWidth)
    {
        m_transVec(0) = -m_transVec(0);
        m_transMat = makeTransMat(m_transVec);
    }
    if(m_center(1)+m_radius+abs(m_transVec(1))+2 > m_winHeight || m_center(1)-m_radius-abs(m_transVec(1))-2 < -m_winHeight)
    {
        m_transVec(1) = -m_transVec(1);
        m_transMat = makeTransMat(m_transVec);
    }
}

Mat3d Clock::makeRotMat(double angle)
{
    Mat3d rotMat;
    rotMat(0,0) = cos(angle);
    rotMat(0,1) = -sin(angle);
    rotMat(1,0) = sin(angle);
    rotMat(1,1) = cos(angle);
    rotMat(2,2) = 1;        //Other entries are 0
    return rotMat;
}

Mat3d Clock::makeTransMat(Vec3d m_transVec)
{
    Mat3d transMat;
    transMat(0,0) = 1;
    transMat(1,1) = 1;
    transMat(2,2) = 1;
    transMat(0,3) = m_transVec(0);
    transMat(1,3) = m_transVec(1);    //Other entries are 0
    return transMat;
}
