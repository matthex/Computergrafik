#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

class Light
{
public:
    Light();

    Light(Vec3d position, Vec3d lightColor, Vec3d ambientColor);

    Vec3d getPosition();

    Vec3d getLightColor();

    Vec3d getAmbient();

private:
    Vec3d m_position;
    Vec3d m_lightColor;
    Vec3d m_ambientColor;
};

#endif // LIGHT_H
