/* 
Copyright 2018 Simon Weis and Philipp Schoenhoefer

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
#include "../duplicationremover.hpp"

void testduplicationremover();


int main()
{
    
    //first some tests for the point class
    testduplicationremover();


    // all tests complete, done
    std::cout << "\n\nall tests complete!" << std::endl;
    return 0;
}


void testduplicationremover()
{
    std::cout << "duplicationremover class: constructor" << std::endl;
    {
        duplicationremover dr(1,1,1);
        pointpattern pp;
        dr.getallPoints(pp);
        assert(pp.points.size() == 0);
    }
    std::cout << "duplicationremover class: constructor" << std::endl;
    {
        duplicationremover dr(1,1,1);
        dr.addpoint(1,2,3, 1, 1);
        pointpattern pp;
        dr.getallPoints(pp);
        assert(pp.points.size() == 1);
    }
    std::cout << "duplicationremover class: removeduplicates" << std::endl;
    {
        duplicationremover dr(1,1,1);
        dr.addpoint(1,2,3, 1, 1);
        dr.removeduplicates(1e-8);
        pointpattern pp;
        dr.getallPoints(pp);
        assert(pp.points.size() == 1);
        dr.addpoint(2,4,6, 2, 1);
        dr.addpoint(1,2,3, 1, 1);
        dr.removeduplicates(1e-8);
        dr.getallPoints(pp);
        assert(pp.points.size() == 2);
    }
    std::cout << "duplicationremover class: removeduplicates" << std::endl;
    {
        duplicationremover dr(100,100,100);
        dr.addpoint(1,2,3, 1, 1);
        dr.removeduplicates(1e-8);
        pointpattern pp;
        dr.getallPoints(pp);
        assert(pp.points.size() == 1);
        dr.addpoint(2,4,6, 2, 1);
        dr.addpoint(1,2,3, 1, 1);
        dr.removeduplicates(1e-8);
        dr.getallPoints(pp);
        assert(pp.points.size() == 2);
    }
}
