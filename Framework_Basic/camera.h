#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

class Camera
{
public:
    Camera();

    void setViewVec(Vec4d viewVec);

    Vec4d getViewVec();

    void setUpVec(Vec4d upVec);

    void setEyePoint(Vec4d eyePoint);

    Vec4d getEyePoint();

    Mat4d getCamMat();

    //Transform world coordinates into camera coordinates
    Mat4d makeTransMat();

    //Transform camera coordinates into world coordinates
    Mat4d makeInverseTransMat();

private:
    Vec4d m_eyepoint;
    Vec4d m_viewVec;
    Vec4d m_upVec;
    Mat4d m_transMat;
    double m_focus;
};

#endif // CAMERA_H
