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
#include "../cmdlparser.hpp"


namespace {
    
class CmdlParserTest : public ::testing::Test
{
public:
    cmdlParser cp;
    
    void parseArguments(std::string const& s)
    {
        std::vector<char*> args;
        convertStringToArgs(s, args);
        cp.parseArguments(args.size(), &args[0]);
    }

    void convertStringToArgs(std::string const& t, std::vector<char*>& args)
    {
        args.clear();
        // for some reason this conversion ignores the first argument. Not going to dig into right now, so the following line is just a quickfix
        std::string s = "a " + t;
        std::istringstream iss(s);

        std::string token;
        while(iss >> token) {
              char* arg = new char[token.size() + 1];
                copy(token.begin(), token.end(), arg);
                  arg[token.size()] = '\0';
                    args.push_back(arg);
        }
    }
};

}

TEST_F(CmdlParserTest, InvalidMode)
{
    EXPECT_THROW(parseArguments("-i test.dat -o outfolder -mode XXX"), std::invalid_argument); 
}

TEST_F(CmdlParserTest, MissingMode)
{
    EXPECT_THROW(parseArguments("-i test.dat -o outfolder -mode"), std::invalid_argument); 
}


TEST_F(CmdlParserTest, modes)
{
    std::map<std::string,eMode> map{
        {"-i test.dat -o outfolder -mode SPHERE", eMode::SPHERE},
        {"-i test.dat -o outfolder -mode sphere", eMode::SPHERE},
        {"-i test.dat -o outfolder -mode SPHEREPOLY", eMode::SPHEREPOLY},
        {"-i test.dat -o outfolder -mode spherepoly", eMode::SPHEREPOLY},
        {"-i test.dat -o outfolder -mode TETRA", eMode::TETRA},
        {"-i test.dat -o outfolder -mode tetra", eMode::TETRA},
        {"-i test.dat -o outfolder -mode TETRABLUNT", eMode::TETRABLUNT},
        {"-i test.dat -o outfolder -mode tetrablunt", eMode::TETRABLUNT},
        {"-i test.dat -o outfolder -mode POLYMER", eMode::POLYMER},
        {"-i test.dat -o outfolder -mode polymer", eMode::POLYMER},
        {"-i test.dat -o outfolder -mode ellip", eMode::ELLIP},
        {"-i test.dat -o outfolder -mode ELLIP", eMode::ELLIP},
        {"-i test.dat -o outfolder -mode SPHCYL", eMode::SPHCYL},
        {"-i test.dat -o outfolder -mode sphcyl", eMode::SPHCYL}
    };

    for(auto const kvp : map)
    {
        EXPECT_NO_THROW(parseArguments(kvp.first));
        EXPECT_EQ(cp.thisMode, kvp.second); 
    }
}

TEST_F(CmdlParserTest, MissingFilename)
{
    EXPECT_THROW(parseArguments("-o outfolder -mode SPHERE -i"), std::invalid_argument); 
}

TEST_F(CmdlParserTest, filename)
{
    std::map<std::string,std::string> map{
        {"-i test.dat -o outfolder -mode SPHERE", "test.dat"},
        {"--i blahurazzzzpumpelpuuu -o outfolder -mode sphere", "blahurazzzzpumpelpuuu"}
    };

    for(auto const kvp : map)
    {
        EXPECT_NO_THROW(parseArguments(kvp.first));
        EXPECT_EQ(cp.filename, kvp.second); 
    }
}

TEST_F(CmdlParserTest, MissingOutputfolder)
{
    EXPECT_THROW(parseArguments("-i test.dat -mode SPHERE -o"), std::invalid_argument); 
}

TEST_F(CmdlParserTest, outfolder)
{
    std::map<std::string,std::string> map{
        {"-i test.dat -o outfolder -mode SPHERE", "outfolder"},
        {"-i test.dat --o blahurazzzzpumpelpuuu -mode sphere", "blahurazzzzpumpelpuuu"}
    };

    for(auto const kvp : map)
    {
        EXPECT_NO_THROW(parseArguments(kvp.first));
        EXPECT_EQ(cp.outfolder, kvp.second); 
    }
}

TEST_F(CmdlParserTest, ArgumentOrder)
{
    std::vector<std::string> vect{
        {"-i in -o out -mode SPHERE"},
        {"-i in -mode SPHERE -o out"},
        {"-o out -i in -mode SPHERE"},
        {"-o out -mode SPHERE -i in"},
        {"-mode SPHERE -i in -o out"},
        {"-mode SPHERE -o out -i in"},
    };

    for(auto const kvp : vect)
    {
        EXPECT_NO_THROW(parseArguments(kvp));
        EXPECT_EQ(cp.thisMode, eMode::SPHERE); 
        EXPECT_EQ(cp.filename, "in"); 
        EXPECT_EQ(cp.outfolder, "out"); 
    }
}
