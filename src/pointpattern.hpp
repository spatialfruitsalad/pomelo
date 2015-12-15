#ifndef POINTPATTERN_GUARD
#define POINTPATTERN_GUARD

#include <vector>

struct point
{
    point(double cx, double cy, double cz, int cl): x(cx), y (cy), z(cz), l(cl) {};
    double x, y, z;
    int l;
};


class pointpattern
{
public:
    void addpoint(double x, double y, double z, int l);
    void print();

    std::vector<point> points;
};

#endif
