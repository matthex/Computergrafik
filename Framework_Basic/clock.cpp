#include "clock.h"

Clock::Clock()
{
    //Nothing to do
}

Clock::Clock(int winWidth, int winHeight, Vec3d center, double radius, Vec3d transVec)
{
    m_center = center;
    m_radius = radius;
    m_longhand = Vec3d(m_center(0), m_center(1)+m_radius*0.9, m_center(2));
    m_shorthand = Vec3d(m_center(0), m_center(1)+m_radius*0.6, m_center(2));

    m_longhandAngle = M_PI/30;  //6°
    m_shorthandAngle = M_PI/6;  //30°
    m_longhandRot = makeRotMat(-m_longhandAngle);
    m_shorthandRot = makeRotMat(-m_shorthandAngle);

    m_transVec = transVec;
    m_transMat = makeTransMat(m_transVec);

    m_longhandCounter = 0;

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
    if(elapsed == 0)
    {
        m_longhand = makeTransMat(m_center)*m_longhandRot*makeTransMat(-m_center)*m_longhand;
        m_longhandCounter++;
        if(m_longhandCounter == 60)
        {
            m_shorthand = makeTransMat(m_center)*m_shorthandRot*makeTransMat(-m_center)*m_shorthand;
            m_longhandCounter = 0;
        }
    }

    //Check collission
    if(m_center(0)+m_radius+2 > m_winWidth || m_center(0)-m_radius-2 < -m_winWidth)
    {
        m_transVec(0) = -m_transVec(0);
        m_transMat = makeTransMat(m_transVec);
    }
    if(m_center(1)+m_radius+2 > m_winHeight || m_center(1)-m_radius-2 < -m_winHeight)
    {
        m_transVec(1) = -m_transVec(1);
        m_transMat = makeTransMat(m_transVec);
    }

    //Move clock
    m_center = m_transMat*m_center;
    m_longhand = m_transMat*m_longhand;
    m_shorthand = m_transMat*m_shorthand;
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
    transMat(0,2) = m_transVec(0);
    transMat(1,2) = m_transVec(1);    //Other entries are 0
    return transMat;
}
