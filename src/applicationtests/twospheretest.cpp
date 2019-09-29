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
#include <fstream>
#include <string.h>


TEST(TwoSphereInput, parameter)
{
    std::ifstream t("../src/inittests/CompDats/parameter.dat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/parameter.dat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSphereInput, custom)
{
    std::ifstream t("../src/inittests/CompDats/custom.dat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/custom.dat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSphereInput, reduced)
{
    std::ifstream t("../src/inittests/CompDats/reduced.xyz");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/reduced.xyz");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSphereInput, triangulation)
{
    std::ifstream t("../src/inittests/CompDats/surface_triangulation.xyz");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/surface_triangulation.xyz");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSpherePostProcessing, volume)
{
    std::ifstream t("../src/inittests/CompDats/setVoronoiVolumes.dat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/setVoronoiVolumes.dat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSpherePostProcessing, area)
{
    std::ifstream t("../src/inittests/CompDats/setVoronoiArea.dat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/setVoronoiArea.dat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSpherePostProcessing, neighbour)
{
    std::ifstream t("../src/inittests/CompDats/setVoronoiNeighbours.dat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/setVoronoiNeighbours.dat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSpherePostProcessing, faces)
{
    std::ifstream t("../src/inittests/CompDats/faces.stat");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/faces.stat");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSphereOutput, poly)
{
    std::ifstream t("../src/inittests/CompDats/cell.poly");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/cell.poly");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

TEST(TwoSphereOutput, fe)
{
    std::ifstream t("../src/inittests/CompDats/cell.fe");
    std::stringstream expectedOutcome;
    expectedOutcome << t.rdbuf();

    std::ifstream in("../test/2015-12-16_twopoints/inittest/cell.fe");
    std::stringstream genfile;
    genfile << in.rdbuf();

    ASSERT_EQ(expectedOutcome.str(),genfile.str()); 
    
}

