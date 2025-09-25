#include "geometry.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

point3d_t corss(point3d_t v1, point3d_t v2)
{
    point3d_t result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

double dot(point3d_t v1, point3d_t v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

bool points_on_same_plane(const point3d_t A, const point3d_t B, const point3d_t C, const point3d_t D)
{

    point3d_t AB = {B.x - A.x, B.y - A.y, B.z - A.z};
    point3d_t AC = {C.x - A.x, C.y - A.y, C.z - A.z};
    point3d_t AD = {D.x - A.x, D.y - A.y, D.z - A.z};

    point3d_t cross_AB_AC = corss(AB, AC);
    double triple_product = dot(cross_AB_AC, AD);

    return triple_product == 0.0;
}

double determinant(double **matrix, int n)
{
    if (n == 1)
    {
        return matrix[0][0];
    }
    if (n == 2)
    {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }

    double det = 0;
    double **submatrix = (double **) malloc((n - 1) * sizeof(double *));
    for (int i = 0; i < n - 1; i++)
    {
        submatrix[i] = (double *) malloc((n - 1) * sizeof(double));
    }

    for (int x = 0; x < n; x++)
    {
        for (int i = 1; i < n; i++)
        {
            int sub_j = 0;
            for (int j = 0; j < n; j++)
            {
                if (j == x)
                    continue;
                submatrix[i - 1][sub_j] = matrix[i][j];
                sub_j++;
            }
        }
        double sign = (x % 2 == 0) ? 1 : -1;
        det += sign * matrix[0][x] * determinant(submatrix, n - 1);
    }

    for (int i = 0; i < n - 1; i++)
    {
        free(submatrix[i]);
    }
    free(submatrix);

    return det;
}

void memset_value_to_matrix4x4(double **m, ...)
{
    va_list args;
    va_start(args, m);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j) m[i][j] = va_arg(args, double);
    va_end(args);
}

void equation_sphere_from_4points(const point3d_t A, const point3d_t B, const point3d_t C, const point3d_t D,
                                  sphere_t *sphere)
{
    if (points_on_same_plane(A, B, C, D))
    {
        fprintf(stderr, "Points are on the same plane\n");
        return;
    }

    double asqrt = pow(A.x, 2) + pow(A.y, 2) + pow(A.z, 2);
    double bsqrt = pow(B.x, 2) + pow(B.y, 2) + pow(B.z, 2);
    double csqrt = pow(C.x, 2) + pow(C.y, 2) + pow(C.z, 2);
    double dsqrt = pow(D.x, 2) + pow(D.y, 2) + pow(D.z, 2);

#define malloc_matrix4x4(x)                                \
    double **x = (double **) malloc(sizeof(double *) * 4); \
    if (!x)                                                \
        return;                                            \
    for (int i = 0; i < 4; ++i)                            \
    {                                                      \
        x[i] = (double *) malloc(sizeof(double) * 4);      \
    }

#define free_matrix4x4(x)                   \
    for (int i = 0; i < 4; ++i) free(x[i]); \
    free(x);

    malloc_matrix4x4(mm11);
    malloc_matrix4x4(mm12);
    malloc_matrix4x4(mm13);
    malloc_matrix4x4(mm14);
    malloc_matrix4x4(mm15);

    memset_value_to_matrix4x4(mm11, A.x, A.y, A.z, 1.0, B.x, B.y, B.z, 1.0, C.x, C.y, C.z, 1.0, D.x, D.y, D.z, 1.0);
    memset_value_to_matrix4x4(mm12, asqrt, A.y, A.z, 1.0, bsqrt, B.y, B.z, 1.0, csqrt, C.y, C.z, 1.0, dsqrt, D.y, D.z,
                              1.0);
    memset_value_to_matrix4x4(mm13, asqrt, A.x, A.z, 1.0, bsqrt, B.x, B.z, 1.0, csqrt, C.x, C.z, 1.0, dsqrt, D.x, D.z,
                              1.0);
    memset_value_to_matrix4x4(mm14, asqrt, A.x, A.y, 1.0, bsqrt, B.x, B.y, 1.0, csqrt, C.x, C.y, 1.0, dsqrt, D.x, D.y,
                              1.0);
    memset_value_to_matrix4x4(mm15, asqrt, A.x, A.y, A.z, bsqrt, B.x, B.y, B.z, csqrt, C.x, C.y, C.z, dsqrt, D.x, D.y,
                              D.z);

    double m11 = determinant(mm11, 4);
    double m12 = determinant(mm12, 4);
    double m13 = determinant(mm13, 4);
    double m14 = determinant(mm14, 4);
    double m15 = determinant(mm15, 4);

    double x0 = 0.5 * m12 / m11;
    double y0 = -0.5 * m13 / m11;
    double z0 = 0.5 * m14 / m11;
    double r = sqrt(x0 * x0 + y0 * y0 + z0 * z0 - m15 / m11);

    free_matrix4x4(mm11);
    free_matrix4x4(mm12);
    free_matrix4x4(mm13);
    free_matrix4x4(mm14);
    free_matrix4x4(mm15);
}

int main(int argc, char **argv)
{
    point3d_t A = {
            .x = 0.0,
            .y = 2.0,
            .z = 0.0,
    };
    point3d_t B = {
            .x = sqrt(3),
            .y = -1.0,
            .z = 0.0,
    };
    point3d_t C = {
            .x = -sqrt(3),
            .y = -1.0,
            .z = 0.0,
    };
    point3d_t D = {
            .x = 0.0,
            .y = 0.0,
            .z = 2.0,
    };

    sphere_t sphere;
    equation_sphere_from_4points(A, B, C, D, &sphere);
    printf("sphere center is: (%f,%f,%f), radius is: %f", sphere.center.x, sphere.center.y, sphere.center.z,
           sphere.radius);
    return 0;
}