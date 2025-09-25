#include "geometry.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double calc_area(circle_t *c, int nc, long nhits);
int inside_circle(double x, double y, circle_t c);

int main(int argc, char **argv)
{
    circle_t circles[2] = {
            (circle_t) {.center = {.x = 0.0, .y = 0.0}, .radius = 1.0},
            (circle_t) {.center = {.x = 1.0, .y = 0.0}, .radius = 1.0}
    };

    int i;
    long hits = 10;
    double area;
    for (i = 0; i < 8; i++)
    {
        area = calc_area(circles, 2, hits);
        hits *= 10;
        printf("Area: %.10lf (hits = %ld)\n", area, hits);
    }

    return 0;
}

double calc_area(circle_t *c, int nc, long nhits)
{
    int inside;
    long n, total = 0, counter = 0, i, j;
    double area, x, y;

    if (nc < 1)
        return (0.0);
    srand(time(0));
    for (i = 0; i < nhits; i++)
    {

#ifdef _WIN32
        x = (2 * (double) (rand()) / (RAND_MAX + 1.0) - 1) * c[0].radius + c[0].center.x;
        y = (2 * (double) (rand()) / (RAND_MAX + 1.0) - 1) * c[0].radius + c[0].center.y;
#else
        x = (2 * drand48() - 1) * c[0].radius + c[0].center.x;
        y = (2 * drand48() - 1) * c[0].radius + c[0].center.y;
#endif

        inside = 1;
        for (j = 0; j < nc; j++)
        {
            if (!inside_circle(x, y, c[j]))
            {
                inside = 0;
                break;
            }
        }
        if (inside)
            counter++;
    }
    area = counter * 4 * c[0].radius * c[0].radius / (double) nhits;

    return (area);
}

/*
   Return TRUE if the point (x,y) is inside the circle.
   Note that the edge is considered inside, see change below.
*/
int inside_circle(double x, double y, circle_t c)
{
    double dy, dx;

    dx = c.center.x - x;
    dy = c.center.y - y;
    if (dx * dx + dy * dy <= c.radius * c.radius) /* Change to < to not include the edge */
        return (1);
    else
        return (0);
}