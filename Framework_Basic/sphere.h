#ifndef SPHERE_H
#define SPHERE_H

#include "Color.h"
#include "vector.h"
#include "QList"

class sphere
{
public:
    sphere();

    sphere(Color color, Vec4d center, double radius);

    Vec3d intersect(Vec3d eye, Vec3d view);

    Vec4d getCenter();

    Vec3d getCenter3();

    void setCenter(Vec4d center);

    Color getColor();

    QList<Vec4d> points;

private:
    Color m_color;
    Vec4d m_center;
    double m_radius;
};

#endif // SPHERE_H
