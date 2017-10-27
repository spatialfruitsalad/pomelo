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
