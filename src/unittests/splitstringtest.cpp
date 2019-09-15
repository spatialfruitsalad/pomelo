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
#include <vector>
#include <sstream>
#include "../splitstring.hpp"

TEST(SplitStringTest, EmptyString)
{
    std::string t = "";
    splitstring ss(t.c_str());
    auto ssv = ss.split('a');
    EXPECT_EQ(ssv.size(), 0);
}

TEST(SplitStringTest, OnlyDelimiters)
{
    std::string t = "----";
    splitstring ss(t.c_str());
    auto ssv = ss.split('-');
    EXPECT_EQ(ssv.size(), 0);
}

TEST(SplitStringTest, WorkingExamples)
{
    std::map<std::string, std::vector<std::string>> map{
        {"- -- -", std::vector<std::string>{" ", " "}},
        {"-a--b-", std::vector<std::string>{"a", "b"}}
    };
    
    for (auto const kvp : map)
    {
        std::string const t = kvp.first;
        splitstring ss(t.c_str());
        auto ssv = ss.split('-');

        EXPECT_EQ(ssv.size(), kvp.second.size());
        EXPECT_EQ(ssv, kvp.second);
    }
}

TEST(SplitStringTest, splitcommandline)
{
    std::map<std::string, std::vector<std::string>> map{
        {"-i test.dat -o outfolder -mode SPHERE", std::vector<std::string>{"-i", "test.dat", "-o", "outfolder", "-mode", "SPHERE"}}
    };
    
    for (auto const kvp : map)
    {
        std::string const t = kvp.first;
        splitstring ss(t.c_str());
        auto ssv = ss.split(' ');

        EXPECT_EQ(ssv.size(), kvp.second.size());
        EXPECT_EQ(ssv, kvp.second);
    }
}


