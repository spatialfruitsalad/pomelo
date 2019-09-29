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

#include "../duplicationremover.hpp"


class DuplicationRemoverTest : public ::testing::TestWithParam<unsigned int>
{
public:
    duplicationremover dr;
    DuplicationRemoverTest() : dr{GetParam(), GetParam(), GetParam()}
    {}
};

TEST(DuplicationRemoverTest, SubdivisionZero)
{
    EXPECT_THROW(duplicationremover dr( 0,1,1) , std::invalid_argument);
    EXPECT_THROW(duplicationremover dr( 1,0,1) , std::invalid_argument);
    EXPECT_THROW(duplicationremover dr( 1,1,0) , std::invalid_argument);
}
TEST_P(DuplicationRemoverTest, SetBoundariesAfterPointAdd)
{
    dr.addpoint(1, 2, 3, 1, 1);
    EXPECT_THROW(dr.setboundaries(0,0,0,0,0,0),std::logic_error);
}

TEST_P(DuplicationRemoverTest, SetBoundaryZero)
{
    EXPECT_THROW(dr.setboundaries(0,0,0,1,0,1), std::invalid_argument);
    EXPECT_THROW(dr.setboundaries(0,1,0,0,0,1), std::invalid_argument);
    EXPECT_THROW(dr.setboundaries(0,1,0,1,0,0), std::invalid_argument);
}

TEST_P(DuplicationRemoverTest, SetInvertedBoundary)
{
    EXPECT_THROW(dr.setboundaries(0,-1,0,1,0,1), std::invalid_argument);
    EXPECT_THROW(dr.setboundaries(0,1,0,-1,0,1), std::invalid_argument);
    EXPECT_THROW(dr.setboundaries(0,1,0,1,0,-1), std::invalid_argument);
}

TEST_P(DuplicationRemoverTest, EmptyDuplicationRemoverResultsInEmptyPointPattern)
{
    // sanity: check that there are no points
    EXPECT_EQ(dr.getallPoints().points.size(), 0);
}

TEST_P(DuplicationRemoverTest, OnePointInDuplicationRemoverResultsInOnePointInPointPattern)
{
    dr.addpoint(1, 2, 3, 1, 1);
    // sanity: check that there are no points
    EXPECT_EQ(dr.getallPoints().points.size(), 1);
}


TEST_P(DuplicationRemoverTest, AddPointIncreasesCountInPointPattern)
{
    const unsigned int N = 50;
    // sanity: check that there are no points
    for (unsigned int i= 0; i != N; ++i)
    {
        const double p = static_cast<double>(i)/static_cast<double>(N);
        dr.addpoint(p,p,p,i,i); 
        EXPECT_EQ(dr.getallPoints().points.size(), i+1);
    }
}


TEST_P(DuplicationRemoverTest, TwoDistinctPointsInDuplicationRemoverResultInTwoPointsInPointPattern)
{
    dr.addpoint(1, 2, 3, 1, 1);
    dr.addpoint(4, 5, 6, 2, 2);
    EXPECT_EQ(dr.getallPoints().points.size(),2);
}


TEST_P(DuplicationRemoverTest, TwoSamePointsInDuplicationRemoverResultInOnePointInPointPattern)
{
    dr.addpoint(1, 2, 3, 1, 1);
    dr.addpoint(1, 2, 3, 1, 1);
    ASSERT_EQ(dr.getallPoints().points.size(), 2);

    // sanity: check that there are two points
    dr.removeduplicates(1e-8);

    EXPECT_EQ(dr.getallPoints().points.size(), 1);
}

INSTANTIATE_TEST_CASE_P(
        DuplicationRemoverTest, DuplicationRemoverTest, ::testing::Values( 1,2,50, 100));
