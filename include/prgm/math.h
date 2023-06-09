#ifndef _MATH_H_
#define _MATH_H_

#define M_PI 3.1415926535

double cos(double);
double sin(double);
double tan(double);

double asin(double);
double acos(double);
double atan(double);
double atan2(double, double);

double cosh(double);
double sinh(double);
double tanh(double);

double exp(double);
double frexp(double, int *);
double ldexp(double, int);
double log(double);
double log10(double);
double modf(double, double *);
double pow(double, double);
double sqrt(double);

#define ceil(X) __builtin_ceil(X)
#define floor(X) __builtin_floor(X)
//double ceil(double);
//double floor(double);

double fabs(double x);
double fmod(double, double);

#endif
