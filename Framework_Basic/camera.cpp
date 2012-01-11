#include "camera.h"
#include "vector.h"
#include "matrix.h"

Camera::Camera()
{
    setUpVec(Vec4d(0,1,0,0));
    setViewVec(Vec4d(0,0,-1,0));
    setEyePoint(Vec4d(0,0,1,0));
}

void Camera::setViewVec(Vec4d viewVec)
{
    //Normalize
    double vecLength = sqrt(viewVec(0)*viewVec(0) + viewVec(1)*viewVec(1) + viewVec(2)*viewVec(2));
    m_viewVec = Vec4d(viewVec(0)/vecLength,viewVec(1)/vecLength,viewVec(2)/vecLength,0);
}

Vec4d Camera::getViewVec()
{
    return m_viewVec;
}

void Camera::setUpVec(Vec4d upVec)
{
    //Normalize
    double vecLength = sqrt(upVec(0)*upVec(0) + upVec(1)*upVec(1) + upVec(2)*upVec(2));
    m_upVec = Vec4d(upVec(0)/vecLength,upVec(1)/vecLength,upVec(2)/vecLength,0);
}

void Camera::setEyePoint(Vec4d eyePoint)
{
    m_eyepoint = eyePoint;
}

Vec4d Camera::getEyePoint()
{
    return m_eyepoint;
}

Mat4d Camera::getCamMat()
{
    return m_transMat;
}

Mat4d Camera::makeTransMat()
{
    Mat4d transMat;

    Vec4d vecS = m_upVec.crossH(m_viewVec);
    double norm = m_upVec.crossH(m_viewVec).length();
    for(int i=0; i<4; i++)
    {
        vecS(i) = vecS(i)/norm;
    }
    Vec4d vecT = m_viewVec.crossH(vecS);

    transMat(0,0) = vecS(0);
    transMat(1,0) = vecS(1);
    transMat(2,0) = vecS(2);

    transMat(0,1) = vecT(0);
    transMat(1,1) = vecT(1);
    transMat(2,1) = vecT(2);

    transMat(0,2) = -m_viewVec(0);
    transMat(1,2) = -m_viewVec(1);
    transMat(2,2) = -m_viewVec(2);

    transMat(3,3) = 1;

    transMat = transMat.makeTransMat(-m_eyepoint)*transMat;    //Translation of eye point to (0,0,0)

    m_transMat = transMat;
    return transMat;
}

Mat4d Camera::makeInverseTransMat()
{
    bool singular;
    Mat4d invTransMat = makeTransMat().inverse(singular);
    return invTransMat;
}
