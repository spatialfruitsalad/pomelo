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

#include <assert.h>
#include "../pointpattern.hpp"

//namespace {
//    
//class PointPatternTest : public ::testing::Test
//{
//public:
//    point p;
//};
//
//}

TEST(PointsTest, Constructor)
{
    point p1(0,0,0,0);
    EXPECT_EQ(p1.x,0); 
    EXPECT_EQ(p1.y,0); 
    EXPECT_EQ(p1.z,0); 
    EXPECT_EQ(p1.l,0); 
    EXPECT_EQ(p1.faceID,-1); 
    EXPECT_EQ(p1.cellID,-1); 

    point p2(1,2,3,1, 99, 100);
    EXPECT_EQ(p2.x,1); 
    EXPECT_EQ(p2.y,2); 
    EXPECT_EQ(p2.z,3); 
    EXPECT_EQ(p2.l,1); 
    EXPECT_EQ(p2.faceID,99); 
    EXPECT_EQ(p2.cellID,100); 
}

TEST(PointsTest, OperatorEq)
{
    point p1(1,2,3,1, 99, 100);
    point p = p1;
    EXPECT_EQ(p.x,1); 
    EXPECT_EQ(p.y,2); 
    EXPECT_EQ(p.z,3); 
    EXPECT_EQ(p.l,1); 
    EXPECT_EQ(p.faceID,99); 
    EXPECT_EQ(p.cellID,100); 
}

TEST(PointsTest, OperatorPlus)
{
    point p1(1,1,1,1, 9, 10);
    point p2(1,2,3,4, 9, 10);
    point p = p1 + p2;
    EXPECT_EQ(p.x,2); 
    EXPECT_EQ(p.y,3); 
    EXPECT_EQ(p.z,4); 
    EXPECT_EQ(p.l,1); 
    EXPECT_EQ(p.faceID,-1); 
    EXPECT_EQ(p.cellID,-1); 
}

TEST(PointsTest, OperatorTimes)
{

    std::vector<double> vect {1.0,2.0,-1.0};
    point p1(1,2,-4,1, 4, 12);

    for(auto const kvp : vect)
    {
        point p=p1*kvp;
        EXPECT_EQ(p.x,kvp); 
        EXPECT_EQ(p.y,2*kvp); 
        EXPECT_EQ(p.z,-4*kvp); 
        EXPECT_EQ(p.l,1); 
        EXPECT_EQ(p.faceID,4); 
        EXPECT_EQ(p.cellID,12); 
        
        p=kvp*p1;
        EXPECT_EQ(p.x,kvp); 
        EXPECT_EQ(p.y,2*kvp); 
        EXPECT_EQ(p.z,-4*kvp); 
        EXPECT_EQ(p.l,1); 
        EXPECT_EQ(p.faceID,4); 
        EXPECT_EQ(p.cellID,12); 
    }

}

TEST(PointsTest, OperatorDivided)
{

    std::vector<double> vect {1.0,2.0,-1.0};
    point p1(2,6,-18,1, 33, 1);

    for(auto const kvp : vect)
    {
        point p=p1/kvp;
        EXPECT_EQ(p.x,2/kvp); 
        EXPECT_EQ(p.y,6/kvp); 
        EXPECT_EQ(p.z,-18/kvp); 
        EXPECT_EQ(p.l,1); 
        EXPECT_EQ(p.faceID,-1); 
        EXPECT_EQ(p.cellID,-1); 
    }
}

TEST(PointsTest, OperatorEqual)
{

    point p1(1,2,3,1, 33, 1);
    point p2(1,2,3,1, 33, 1);
    point p3(4,22,5,9, 3, 10);


    EXPECT_TRUE(p1==p2); 
    EXPECT_FALSE(p1==p3); 
}

TEST(PointsTest, OperatorUnequal)
{

    point p1(1,2,3,1, 33, 1);
    point p2(4,22,5,9, 3, 10);
    point p3(1,2,3,1, 33, 1);


    EXPECT_TRUE(p1!=p2); 
    EXPECT_FALSE(p1!=p3); 
}

TEST(PointsTest, Length)
{

    point p1(0,0,0,1);
    EXPECT_EQ(p1.length(),0); 

    point p2(4,9,-16,1);
    EXPECT_EQ(p2.length(),sqrt(4*4+9*9+16*16)); 
}


TEST(PointPatternTest, AddPoint)
{

    pointpattern pp;
    EXPECT_EQ(pp.points.size(),0); 

    pp.addpoint(1, 2,3,4); 
    EXPECT_EQ(pp.points.size(),1); 
    
    pp.clear();
    EXPECT_EQ(pp.points.size(),0); 

    for (unsigned long i = 0; i != 10000; ++i)
    {
        pp.addpoint(i, i/2.0, i/3.0, i/4.0);
    }
    EXPECT_EQ(pp.points.size(),10000); 

    pp.clear();
    EXPECT_EQ(pp.points.size(),0); 
}

TEST(PointPatternTest, AddPointForCell)
{
    pointpattern pp;
    EXPECT_EQ(pp.points.size(),0); 

    pp.addpointForCell(1, 2,3,4,1,8); 
    EXPECT_EQ(pp.points.size(),1); 
    EXPECT_EQ(pp.points[0].faceID,1);
    EXPECT_EQ(pp.points[0].cellID,8);

    pp.clear();
    EXPECT_EQ(pp.points.size(),0); 
}

TEST(PointPatternTest, RemoveDuplicates)
{
    pointpattern pp;
    pp.addpoint(1, 2,3,4); 
    pp.addpoint(2, 2,3,4); 
    EXPECT_EQ(pp.points.size(),2); 
    pp.removeduplicates(1e-8);
    EXPECT_EQ(pp.points.size(),1); 

    pp.addpoint(3, 5,5,5); 
    EXPECT_EQ(pp.points.size(),2); 
    pp.removeduplicates(1e-8);
    EXPECT_EQ(pp.points.size(),2); 
}
