#include "sphere.h"
#include "math.h"

sphere::sphere()
{
}

sphere::sphere(Color color, Vec4d center, double radius)
{
    m_color = color;
    m_center = center;
    m_radius = radius;

        for(int i=-180; i<=180; i++)
        {
            for(int j=0; j<=180; j++)
            {
                double x = m_radius * sin(j) * cos(i);
                double y = m_radius * sin(j) * sin(i);
                double z = m_radius * cos(j);
                points.append(Vec4d(x, y, z, 1));
            }
        }
}

sphere::sphere(Material material, Vec4d center, double radius)
{
    m_material = material;
    m_center = center;
    m_radius = radius;
}

Vec3d sphere::intersect(Vec3d eye, Vec3d view)
{
    double a = view * view;
    double b = view * (eye - getCenter3()) * 2;
    double c = (eye - getCenter3()) * (eye - getCenter3()) - m_radius * m_radius;

    double det = b*b - 4*a*c;

    if (det>=0){
        double t1 = (-b + sqrt(det)) / 2*a;     // abc formula
        double t2 = (-b - sqrt(det)) / 2*a;

        Vec3d s1 = eye + view*t1;
        Vec3d s2 = eye + view*t2;

        if(t1>0 || t2>0)
        {
            if(t1>0 && t2>0 && s1(2)>s2(2))
            {
                return s1;
            }
            else
            {
                return s2;
            }
            if(t1>0)
            {
                return s1;
            }
            else
            {
                return s2;
            }
        }
    }
    return Vec3d(0,0,-INFINITY);
}


Vec4d sphere::getCenter()
{
    return m_center;
}

Vec3d sphere::getCenter3()
{
    Vec3d center;
    center(0) = m_center(0);
    center(1) = m_center(1);
    center(2) = m_center(2);
    return center;
}

void sphere::setCenter(Vec4d center)
{
    m_center = center;
}

Color sphere::getColor()
{
    return m_color;
}

Material sphere::getMaterial()
{
    return m_material;
}
