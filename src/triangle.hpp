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
#ifndef TRIANGLE_GUARD_
#define TRIANGLE_GUARD_

#include <iomanip>
#include <vector>
#include <map>
#include <cmath>
#include "pointpattern.hpp"

class triangle
{
public:
    triangle(point px, point py, point pz): p{ px,  py , pz}
    {
    };

    point p[3];

    
    void getPlaneEquationCoefficients(double& a, double& b, double& c, double& d)
    {
        const double e1x = p[1].x - p[0].x;
        const double e1y = p[1].y - p[0].y;
        const double e1z = p[1].z - p[0].z;

        const double e2x = p[2].x - p[0].x;
        const double e2y = p[2].y - p[0].y;
        const double e2z = p[2].z - p[0].z;

        const double nx = e1y*e2z - e1z*e2y;
        const double ny = e1z*e2x - e1x*e2z;
        const double nz = e1x*e2y - e1y*e2x;
        
        const double l = sqrt(nx*nx + ny*ny + nz*nz);


        a = nx/l;
        b = ny/l;
        c = nz/l;

        d = -a * p[0].x - b * p[0].y - c*p[0].z;
    }

    static void recusiveSubdivide (unsigned int depth, std::vector<triangle>& tris)
    {
        if (tris.size() == 0) return;

        for(unsigned int i = 0; i != depth; ++i)
        {
            std::vector<triangle> tris2;
            for (auto it = tris.begin(); it != tris.end(); ++it)
            {
                triangle t = *it;
                auto sub = t.subdivide();
                tris2.insert(tris2.end(), sub.begin(), sub.end());
            }
            std::swap(tris, tris2);
        }
    };


    std::vector<triangle> subdivide()
    {
        std::vector<triangle> ret;
        ret.clear();

        point pmid[3]{ 0.5*(p[0] + p[1]), 0.5*(p[1] + p[2]), 0.5*(p[2] + p[0])}; 

        triangle t1 (p[0], pmid[0], pmid[2]);
        triangle t2 (p[1], pmid[1], pmid[0]);
        triangle t3 (p[2], pmid[2], pmid[1]);
        triangle t4 (pmid[0], pmid[1], pmid[2]);

        ret.push_back(t1);
        ret.push_back(t2);
        ret.push_back(t3);
        ret.push_back(t4);

        return ret;
    };
};


#endif
