#include <iostream>
#include "pointpattern.hpp"


void pointpattern::addpoint(double x, double y, double z, int l)
{
    point p (x,y,z,l);
    points.push_back(p);
}

void pointpattern::print ()
{
    std::cout << "number of points " << points.size() << std::endl;

}
