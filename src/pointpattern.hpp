#ifndef POINTPATTERN_GUARD
#define POINTPATTERN_GUARD

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
struct point
{
    point(double cx, double cy, double cz, int cl, long cf = -1, long cC = -1): x(cx), y (cy), z(cz), l(cl), faceID(cf), cellID(cC) {};
    double x, y, z;
    int l;
    long faceID, cellID;
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
    void addpointForCell(double x, double y, double z, int l, long cf, long cC);
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

    friend std::ostream& operator >> (std::ostream &f, const pointpattern& p)
    {
        if(p.points.empty())
            return f;
	double xx = p.points[0].x;
	double yy = p.points[0].y;
	double zz = p.points[0].z;
        int oldf = p.points[0].faceID;
	int oldc = p.points[0].cellID;
        for (   auto it = p.points.begin();
                it != p.points.end();
                ++it)
        {
            if(oldf != (*it).faceID)
            {
                f << oldc << " " <<  std::setw(5)<< xx << " " << std::setw(5) << yy << " " << std::setw(5) << zz << "\n\n";
                if(oldc != (*it).cellID) f << "\n";
		oldf = (*it).faceID;
		oldc = (*it).cellID;
		xx = (*it).x;
		yy = (*it).y;
		zz = (*it).z;
            }
            f << (*it).cellID << " " <<  std::setw(5)<< (*it).x << " " << std::setw(5) << (*it).y << " " << std::setw(5) << (*it).z << "\n";
        }

        return f;
    };
};

#endif
