/* 
Copyright 2019 Simon Weis and Philipp Schoenhoefer

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

#include <gtest/gtest.h>
#include "../triangle.hpp"

TEST(TriangleTest, Constructor)
{
    point const p1(1,2,3,1);
    point const p2(-1,-2,-3,1);
    point const p3(3,2,1,1);
    triangle t (p1,p2,p3);
    EXPECT_EQ(t.p[0], p1); 
    EXPECT_EQ(t.p[1], p2);
    EXPECT_EQ(t.p[2], p3); 
}


TEST(TriangleTest, CalculatePlaneEquationDMinusOne)
{
    point const p1(1,2,3,1);
    point const p2(1,-2,-3,1);
    point const p3(1,2,1,1);
    triangle t (p1,p2,p3);
    double nx, ny, nz, d;
    t.getPlaneEquationCoefficients(nx,ny,nz,d);
    EXPECT_EQ(nx , 1);
    EXPECT_EQ(ny , 0);
    EXPECT_EQ(nz , 0);
    EXPECT_EQ(d , -1);
}

TEST(TriangleTest, CalculatePlaneEquationDOne)
{
    point const p1(1,2,1,1);
    point const p2(1,-2,-3,1);
    point const p3(1,2,3,1);
    triangle t (p1,p2,p3);
    double nx, ny, nz, d;
    t.getPlaneEquationCoefficients(nx,ny,nz,d);
    EXPECT_EQ(nx , -1);
    EXPECT_EQ(ny , 0);
    EXPECT_EQ(nz , 0);
    EXPECT_EQ(d , 1);
}

TEST(TriangleTest, CalculatePlaneEquationSquareRootThree)
{
    triangle t ( point(1,0,0,1), point(0,1,0,1), point(0,0,1,1) );
    double nx, ny, nz, d;
    t.getPlaneEquationCoefficients(nx,ny,nz,d);
    EXPECT_EQ(nx , 1.0/sqrt(3));
    EXPECT_EQ(ny , 1.0/sqrt(3));
    EXPECT_EQ(nz , 1.0/sqrt(3));
    EXPECT_EQ(d , -1.0/sqrt(3));
}


