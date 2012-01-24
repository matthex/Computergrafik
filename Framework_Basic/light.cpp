#include "light.h"

Light::Light()
{

}

Light::Light(Vec3d position, Vec3d lightColor, Vec3d ambientColor)
{
    m_position = position;
    m_lightColor = lightColor;
    m_ambientColor = ambientColor;
}

Vec3d Light::getPosition()
{
    return m_position;
}

Vec3d Light::getLightColor()
{
    return m_lightColor;
}

Vec3d Light::getAmbient()
{
    return m_ambientColor;
}
