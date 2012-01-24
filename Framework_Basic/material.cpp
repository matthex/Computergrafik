#include "material.h"

Material::Material()
{
}

Material::Material(Vec3d diffuse, Vec3d specular, Vec3d ambient, double shininess)
{
    m_diffuse = diffuse;
    m_specular = specular;
    m_ambient = ambient;
    m_shininess = shininess;
}

Vec3d Material::getDiffuse()
{
    return m_diffuse;
}

Vec3d Material::getAmbient()
{
    return m_ambient;
}

Vec3d Material::getSpecular()
{
    return m_specular;
}

double Material::getShininess()
{
    return m_shininess;
}
