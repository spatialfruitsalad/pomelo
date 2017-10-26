/* 
Copyright 2016 Simon Weis and Philipp Schoenhoefer

This file is part of Pomelo.

Pomelo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pomelo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pomelo.  If not, see <http://www.gnu.org/licenses/>.

The development of Pomelo took place at the Friedrich-Alexander University of Erlangen and was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS). 
*/
#include <iostream>
#include "pointpattern.hpp"


void pointpattern::addpoint(const int l, const double x, const double y, const double z)
{
    //std::cout << "adding point " << l << " " << x << " " << y << " " << z << std::endl;
    point p (x,y,z,l);
    points.push_back(p);
}

void pointpattern::addpointForCell(const double x, const double y, const double z, const int l, const long cf, const long cC)
{
    point p (x,y,z,l, cf,cC);
    points.push_back(p);
}


void pointpattern::print () const
{
    std::cout << "number of points " << points.size() << std::endl;
}

void pointpattern::removeduplicates (const double epsilon)
{
    if(points.empty()) return;
    std::vector<point> newpoints;

    for(unsigned int i = 0; i != points.size(); ++i)
    {
        bool addthis =true;
        const point p1 = points[i];
        point p2(0,0,0,-1,-1);
        for(unsigned int j = i+1; j != points.size(); ++j)
        {
            if (j >= points.size()) break;
            p2 = points[j];
            if (checkdistancecloserthan(p1, p2, epsilon))
            {
                if (p1.cellID == p2.cellID && p1.l != -1)
                {
                    addthis = false;
                    break;
                }
            }
        }
        if (addthis)
        {
            unsigned int l = p1.l;
            indexShift[l] = -1;
            //std::cout << "adding" << std::endl;
            newpoints.push_back(p1);
        }
        else
        {
            const unsigned int l1 = p1.l;
            const unsigned int l2 = p2.l;
            indexShift[l1] = l2;
        }
    }
    points = newpoints;
}

void pointpattern::removeduplicates (const double epsilon, pointpattern& p)
{
    if(points.empty()) return;
    std::vector<point> newpoints;
    for(unsigned int i = 0; i != points.size(); ++i)
    {
        bool addthis =true;
        const point p1 = points[i];
        point p2(0,0,0,-1, -1);
        for(unsigned int j = 0; j != p.points.size(); ++j)
        {
            p2 = p.points[j];
            if (checkdistancecloserthan(p1, p2, epsilon))
            {
                if (p1.cellID == p2.cellID && p1.l != -1)
                {
                    addthis = false;
                    break;
                }
            }
        }
        if (addthis)
        {
            newpoints.push_back(p1);
            const unsigned int l = p1.l;
            indexShift[l] = -1;
        }
        else
        {
            const unsigned int l1 = p1.l;
            const unsigned int l2 = p2.l;
            indexShift[l1] = l2;
        }
    }
    points = newpoints;
}

point operator/ (const point& p,  double const& f)
{
    return point(p.x /f, p.y / f, p.z /f, p.l);
}

point operator* ( double const& f, const point& p)
{
    return point(p.x *f, p.y * f, p.z *f, p.l, p.faceID, p.cellID);
}

point operator* (const point& p,  double const& f)
{
    return point(p.x *f, p.y * f, p.z *f, p.l, p.faceID, p.cellID);
}
