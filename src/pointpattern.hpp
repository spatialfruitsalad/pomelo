#ifndef POINTPATTERN_GUARD
#define POINTPATTERN_GUARD

#include <iomanip>
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
    
    friend std::ostream& operator << (std::ostream &f, const pointpattern& p)
    {
        for (   auto it = p.points.begin();
                it != p.points.end();
                ++it)
        {
            f << (*it).l << " " <<  std::setw(5)<< (*it).x << " " << std::setw(5) << (*it).y << " " << std::setw(5) << (*it).z << "\n";
        }

        return f;
    };
};

#endif
