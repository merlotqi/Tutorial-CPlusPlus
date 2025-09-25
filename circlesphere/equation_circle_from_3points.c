#include "geometry.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

// Check the given point are perpendicular to x or y axis
int is_perpendicular(const point2d_t p1, const point2d_t p2, const point2d_t p3)
{
    double yDelta_a = p2.y - p1.y;
    double xDelta_a = p2.x - p1.x;
    double yDelta_b = p3.y - p2.y;
    double xDelta_b = p3.x - p2.x;

    // checking whether the line of the two pts are vertical
    if (fabs(xDelta_a) <= 0.000000001 && fabs(yDelta_b) <= 0.000000001)
    {
        fprintf(stderr, "The points are pependicular and parallel to x-y axis");
        return -1;
    }

    if (fabs(yDelta_a) <= 0.0000001)
    {
        return 0;
    }
    else if (fabs(yDelta_b) <= 0.0000001)
    {
        return 0;
    }
    else if (fabs(xDelta_a) <= 0.000000001)
    {
        return 0;
    }
    else if (fabs(xDelta_b) <= 0.000000001)
    {
        return 0;
    }
    else
        return -1;
}

double calc_circle(const point2d_t p1, const point2d_t p2, const point2d_t p3, circle_t *c)
{
    assert(c);

    double yDelta_a = p2.y - p1.y;
    double xDelta_a = p2.x - p1.x;
    double yDelta_b = p3.y - p2.y;
    double xDelta_b = p3.x - p2.x;

    if (fabs(xDelta_a) <= 0.000000001 && fabs(yDelta_b) <= 0.000000001)
    {
        printf("Calc cirlce \n");
        c->center.x = 0.5 * (p2.x + p3.x);
        c->center.y = 0.5 * (p1.y + p2.y);
        c->radius = point2d_distance(c->center, p1);// calc. radius
        return c->radius;
    }

    // is_perpendicular() assure that xDelta(s) are not zero
    double aSlope = yDelta_a / xDelta_a;//
    double bSlope = yDelta_b / xDelta_b;
    if (fabs(aSlope - bSlope) <= 0.000000001)
    {
        // checking whether the given points are colinear.
        fprintf(stderr, "The three pts are colinear\n");
        return -1;
    }

    // calc center
    c->center.x = (aSlope * bSlope * (p1.y - p3.y) + bSlope * (p1.x + p2.x) - aSlope * (p2.x + p3.x)) /
                  (2 * (bSlope - aSlope));
    c->center.y = -1 * (c->center.x - (p1.x + p2.x) / 2) / aSlope + (p1.y + p2.y) / 2;

    c->radius = point2d_distance(c->center, p1);// calc. radius
    return c->radius;
}

void equation_circle_from_3point(const point2d_t p1, const point2d_t p2, const point2d_t p3, circle_t *c)
{
    assert(c);
    c->radius = -1;

    if (0 != is_perpendicular(p1, p2, p3))
        calc_circle(p1, p2, p3, c);
    else if (0 != is_perpendicular(p1, p3, p2))
        calc_circle(p1, p3, p2, c);
    else if (0 != is_perpendicular(p2, p1, p3))
        calc_circle(p2, p1, p3, c);
    else if (0 != is_perpendicular(p2, p3, p1))
        calc_circle(p2, p3, p1, c);
    else if (0 != is_perpendicular(p3, p2, p1))
        calc_circle(p3, p2, p1, c);
    else if (0 != is_perpendicular(p3, p1, p2))
        calc_circle(p3, p1, p2, c);
    else
    {
        fprintf(stderr, "The three pts are perpendicular to axis");

        return;
    }
}

int main(int argc, char *argv[])
{

    circle_t circle;
    equation_circle_from_3point((point2d_t) {.x = -1, .y = 0}, (point2d_t) {.x = 0, .y = 2},
                                (point2d_t) {.x = 1, .y = 0}, &circle);
    printf("circle: center is (%f, %f), radius is %f \n", circle.center.x, circle.center.y, circle.radius);
    return 0;
}