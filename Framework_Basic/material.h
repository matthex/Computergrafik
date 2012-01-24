#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"

class Material
{
public:
    Material();

    Material(Vec3d diffuse, Vec3d specular, Vec3d ambient, double shininess);

    Vec3d getAmbient();

    Vec3d getDiffuse();

    Vec3d getSpecular();

    double getShininess();

private:
    Vec3d m_diffuse;
    Vec3d m_specular;
    Vec3d m_ambient;
    double m_shininess;
};

#endif // MATERIAL_H
