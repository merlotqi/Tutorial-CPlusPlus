#include "geometry.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

void intersection_2_circles(const circle_t c1, const circle_t c2, point2d_t *ips, int *ipsn)
{
    assert(ips);
    assert(ipsn);

    /* Determine the straight-line distance between the centers. */
    double dx = c2.center.x - c1.center.x;
    double dy = c2.center.y - c1.center.y;
    double d = hypot(dx, dy);

    /* check for solvability */
    if (d > (c1.radius + c2.radius))
    {
        /* no solution. circle do not intersects. */
        return;
    }
    if (d < fabs(c1.radius - c2.radius))
    {
        /* no solution. one circle is contained in the other */
        return;
    }

    double a = (c1.radius * c1.radius - c2.radius * c2.radius + d * d) / (2.0 * d);

    double x2 = c1.center.x + (dx * a / d);
    double y2 = c1.center.y + (dy * a / d);
    /* Determine the distance from point 2 to either of the
   * intersection points.
   */
    double h = sqrt((c1.radius * c1.radius) - (a * a));

    /* Now determine the offsets of the intersection points from
   * point 2.
   */
    double rx = -dy * (h / d);
    double ry = dx * (h / d);

    /* Determine the absolute intersection points. */
    ips[0].x = x2 + rx;
    ips[0].y = y2 + ry;
    ips[1].x = x2 - rx;
    ips[1].y = y2 - ry;
    ipsn = 2;
}