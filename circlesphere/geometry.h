#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>

typedef struct point2d_s
{
    double x;
    double y;
} point2d_t;

typedef struct point3d_s
{
    double x;
    double y;
    double z;
} point3d_t;

typedef struct circle_s
{
    point2d_t center;
    double radius;
} circle_t;

typedef struct sphere_s
{
    point3d_t center;
    double radius;
} sphere_t;

double point2d_distance(const point2d_t p1, const point2d_t p2)
{
    return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

#endif