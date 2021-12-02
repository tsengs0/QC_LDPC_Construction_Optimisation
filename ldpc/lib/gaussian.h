#ifndef GAUSSIAN_H_INCLUDED
#define GAUSSIAN_H_INCLUDED
/*
    Using Box-Muller transform to obtain the standard Gaussian Distribution from the (0,1) uniform Distribution
*/

#define PI 3.14159265359

#include <stdlib.h>
#include <math.h>
#include <twister.h>


double Gaussian();


inline double Gaussian()
{
    double U = 0, V = 0, z = 0;
    U = randomMT() / (Max_RandMT + 1.0);
    V = randomMT() / (Max_RandMT + 1.0);
    z = sqrt(-2.0 * log(U)) * sin(2.0 * PI * V);

    return z;
}

/*
long double rand_normal(long double mean, long double stddev)
{//Box muller method
    static long double n2 = 0.0;
    static int n2_cached = 0;
    if (!n2_cached)
    {
        long double x, y, r;
        do
        {
            x = 2.0*rand()/RAND_MAX - 1;
            y = 2.0*rand()/RAND_MAX - 1;

            r = x*x + y*y;
        }
        while (r == 0.0 || r > 1.0);
        {
            long double d = sqrt(-2.0*log(r)/r);
            long double n1 = x*d;
            n2 = y*d;
            long double result = n1*stddev + mean;
            n2_cached = 1;
            return result;
        }
    }
    else
    {
        n2_cached = 0;
        return n2*stddev + mean;
    }
}
*/

#endif // GAUSSIAN_H_INCLUDED