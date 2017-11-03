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
#include <assert.h>
#include "../triangle.hpp"

void testtriangle();


int main()
{
    std::cout << "test triangle\n";

    testtriangle();

    return 0;
}

void testtriangle()
{
    std::cout << "triangle: constructor\n";
    {
        triangle t (point(1,2,3,1), point(-1,-2,-3,1), point(3,2,1,1));
        assert(t.p[0].x == 1);
        assert(t.p[1].x == -1);
        assert(t.p[2].x == 3);
    }

    std::cout << "triangle: get plane equation\n";
    {
        triangle t (point(1,2,3,1), point(1,-2,-3,1), point(1,2,1,1));
        double nx, ny, nz, d;
        t.getPlaneEquationCoefficients(nx,ny,nz,d);
        //std::cout << nx << " " << ny << " " << nz << " " << d << std::endl; 
        assert(nx == 1);
        assert(ny == 0);
        assert(nz == 0);
        assert(d == -1);
    }
    {
        triangle t ( point(1,2,1,1), point(1,-2,-3,1), point(1,2,3,1) );
        double nx, ny, nz, d;
        t.getPlaneEquationCoefficients(nx,ny,nz,d);
        //std::cout << nx << " " << ny << " " << nz << " " << d << std::endl; 
        assert(nx == -1);
        assert(ny == 0);
        assert(nz == 0);
        assert(d == 1);
    }
    {
        triangle t ( point(1,2,1,1), point(-2,2,-3,1), point(1,2,3,1) );
        double nx, ny, nz, d;
        t.getPlaneEquationCoefficients(nx,ny,nz,d);
        assert(nx == 0);
        assert(ny == 1);
        assert(nz == 0);
        assert(d == -2);
    }
    {
        triangle t ( point(2,1,-5,1), point(-2,2,-5,1), point(1,2,-5,1) );
        double nx, ny, nz, d;
        t.getPlaneEquationCoefficients(nx,ny,nz,d);
        assert(nx == 0);
        assert(ny == 0);
        assert(nz == -1);
        assert(d == -5);
    }
    {
        triangle t ( point(1,0,0,1), point(0,1,0,1), point(0,0,1,1) );
        double nx, ny, nz, d;
        t.getPlaneEquationCoefficients(nx,ny,nz,d);
        assert(nx == 1.0/sqrt(3));
        assert(ny == 1.0/sqrt(3));
        assert(nz == 1.0/sqrt(3));
        assert(d == - 1.0/sqrt(3));
    }
}
