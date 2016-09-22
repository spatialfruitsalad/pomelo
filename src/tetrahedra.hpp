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
#ifndef TETRAHEDRA_GUARD_
#define TETRAHEDRA_GUARD_

#include <iomanip>
#include <vector>
#include <fstream>
#include <map>
#include <cmath>
#include "triangle.hpp"

class tetrahedra
{
public:
    tetrahedra (point p1, point p2, point p3, point p4)
    {
        label = p1.l; 
        // create triangles
        triangle t1(p1, p2, p3);   
        triangle t2(p2, p1, p4);   
        triangle t3(p3, p2, p4);   
        triangle t4(p1, p3, p4);   
        t1.getPlaneEquationCoefficients(a1, b1,c1,d1);
        t2.getPlaneEquationCoefficients(a2, b2,c2,d2);
        t3.getPlaneEquationCoefficients(a3, b3,c3,d3);
        t4.getPlaneEquationCoefficients(a4, b4,c4,d4);
        
        {
            std::ofstream fpoints("points.dat");
            fpoints << p1.x << " " << p1.y << " " << p1.z << std::endl; 
            fpoints << p2.x << " " << p2.y << " " << p2.z << std::endl; 
            fpoints << p3.x << " " << p3.y << " " << p3.z << std::endl; 
            fpoints << p4.x << " " << p4.y << " " << p4.z << std::endl; 
            fpoints.close();
        }

        centerX = (p1.x + p2.x + p3.x + p4.x)/4.0;
        centerY = (p1.y + p2.y + p3.y + p4.y)/4.0;
        centerZ = (p1.z + p2.z + p3.z + p4.z)/4.0;


        {
            std::ofstream fnormals("normals.dat");
            fnormals << centerX << " " << centerY << " " << centerZ << std::endl;
            fnormals << centerX - a1 << " " << centerY - b1 << " " << centerZ - c1 << " " << d1 << std::endl << std::endl<< std::endl; 

            fnormals << centerX << " " << centerY << " " << centerZ << std::endl;
            fnormals << centerX - a2 << " " << centerY - b2 << " " << centerZ - c2 << " " << d2 << std::endl << std::endl<< std::endl; 
            
            fnormals << centerX << " " << centerY << " " << centerZ << std::endl;
            fnormals << centerX - a3 << " " << centerY - b3 << " " << centerZ - c3 << " " << d3 << std::endl << std::endl<< std::endl; 
            
            fnormals << centerX << " " << centerY << " " << centerZ << std::endl;
            fnormals << centerX - a4 << " " << centerY - b4 << " " << centerZ - c4 << " " << d4 << std::endl << std::endl<< std::endl; 
            fnormals.close();
        }

        point com(centerX, centerY, centerZ +1 , 1);
        std::cout << "check com inside: " << (checkPointInside(com) ? "true" : "false") << std::endl;
        std::cout << "check p1 inside: " << (checkPointInside(p1) ? "true" : "false") << std::endl;
        std::cout << "check p2 inside: " << (checkPointInside(p2) ? "true" : "false") << std::endl;
        std::cout << "check p3 inside: " << (checkPointInside(p3) ? "true" : "false") << std::endl;
        std::cout << "check p4 inside: " << (checkPointInside(p4) ? "true" : "false") << std::endl;
        point origin (0,0,0, 1);
        std::cout << "check origin inside: " << (checkPointInside(origin) ? "true" : "false") << std::endl;
    }

    unsigned long label;
    double a1, a2, a3, a4;
    double b1, b2, b3, b4;
    double c1, c2, c3, c4;
    double d1, d2, d3, d4;
    
    double centerX, centerY, centerZ;
    


    bool checkPointInside( point p)
    {
        int s1 = (a1*p.x + b1*p.y + c1*p.z + d1 > 0) ? 1 : -1;
        int s2 = (a2*p.x + b2*p.y + c2*p.z + d2 > 0) ? 1 : -1;
        int s3 = (a3*p.x + b3*p.y + c3*p.z + d3 > 0) ? 1 : -1;
        int s4 = (a4*p.x + b4*p.y + c4*p.z + d4 > 0) ? 1 : -1;

       return s1 == s2 == s3 == s4; 
    }
};

#endif
