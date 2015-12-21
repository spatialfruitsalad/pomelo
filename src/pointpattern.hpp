#ifndef POINTPATTERN_GUARD
#define POINTPATTERN_GUARD

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
struct point
{
    point(double cx, double cy, double cz, int cl): x(cx), y (cy), z(cz), l(cl) {};
    double x, y, z;
    int l;
};

inline bool checkdistancecloserthan (point& a, point& b, double e)
{
    double dx =fabs(a.x - b.x);
    double dy =fabs(a.y - b.y);
    double dz =fabs(a.z - b.z);

    return (dx < e && dy < e && dz < e);
};

class pointpattern
{
public:
    void addpoint(double x, double y, double z, int l);
    void print();
    void removeduplicates ( double epsilon);
    void removeduplicates ( double epsilon, pointpattern& p);
    std::vector<point> points;
    std::map<unsigned int, long> indexShift;    // first is particle ID, second is new Index or -1

    friend std::ostream& operator << (std::ostream &f, const pointpattern& p)
    {
        if(p.points.empty())
            return f;
        int oldl = p.points[0].l;
        for (   auto it = p.points.begin();
                it != p.points.end();
                ++it)
        {
            if(oldl != (*it).l )
            {
                f << "\n\n";
                oldl = (*it).l;
            }
            f << (*it).l << " " <<  std::setw(5)<< (*it).x << " " << std::setw(5) << (*it).y << " " << std::setw(5) << (*it).z << "\n";
        }

        return f;
    };
};

#endif
