#include <iostream>
#include <assert.h>
#include "../pointpattern.hpp"

void pointStruct();
void pointPatternClass(); 


int main()
{
    
    //first some tests for the point class
    pointStruct();

    pointPatternClass();

    // all tests complete, done
    std::cout << "\n\nall tests complete!" << std::endl;
    return 0;
}

void pointStruct()
{
    std::cout << "struct point: constructor\n";
    {
        point p(0,0,0,0);
        assert(p.x == 0);
        assert(p.y == 0);
        assert(p.z == 0);
        assert(p.l == 0);
        assert(p.faceID == -1);
        assert(p.cellID == -1);
    }
    {
        point p(1,2,3,1, 99, 100);
        assert(p.x == 1);
        assert(p.y == 2);
        assert(p.z == 3);
        assert(p.l == 1);
        assert(p.faceID == 99);
        assert(p.cellID == 100);
    }
    std::cout << "struct point: operator=\n";
    {
        point p1(5,5,5,5, 5, 5);
        point p = p1;
        assert(p.x == 5);
        assert(p.y == 5);
        assert(p.z == 5);
        assert(p.l == 5);
        assert(p.faceID == 5);
        assert(p.cellID == 5);
    }

    std::cout << "struct point: operator+\n";
    {
        point p1(5,5,5,1, 5, 5);
        point p2(3,3,3,2, 7,8);

        point p = p1 + p2;

        assert(p.x == 8);
        assert(p.y == 8);
        assert(p.z == 8);
        assert(p.l == 1);
        assert(p.faceID == -1);
        assert(p.cellID == -1);
    }
    std::cout << "struct point: operator*\n";
    {
        point p1(1,2,-4, 1, 6, 3);

        {
            point p = p1 * 1.0;
            assert(p.x == 1);
            assert(p.y == 2);
            assert(p.z == -4);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }
        {
            point p = p1 * 2.0;
            assert(p.x == 2);
            assert(p.y == 4);
            assert(p.z == -8);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }
        {
            point p = p1 * -1.0;
            assert(p.x == -1);
            assert(p.y == -2);
            assert(p.z == 4);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }

        {
            point p = 1.0* p1 ;
            assert(p.x == 1);
            assert(p.y == 2);
            assert(p.z == -4);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }
        {
            point p = 2.0 * p1;
            assert(p.x == 2);
            assert(p.y == 4);
            assert(p.z == -8);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }
        {
            point p = -1.0 * p1 ;
            assert(p.x == -1);
            assert(p.y == -2);
            assert(p.z == 4);
            assert(p.l == 1);
            assert(p.faceID == 6);
            assert(p.cellID == 3);
        }
    }

    std::cout << "struct point: operator/\n";
    {
        point p1(6,12,24, 1, 6, 3);

        {
            point p = p1 / 1.0;
            assert(p.x == 6);
            assert(p.y == 12);
            assert(p.z == 24);
            assert(p.l == 1);
            assert(p.faceID == -1);
            assert(p.cellID == -1);
        }
        {
            point p = p1 / 2.0;
            assert(p.x == 3);
            assert(p.y == 6);
            assert(p.z == 12);
            assert(p.l == 1);
            assert(p.faceID == -1);
            assert(p.cellID == -1);
        }
    }

    std::cout << "struct point: length()\n";
    {
        point p1(0,0,0,1);
        assert(p1.length() == 0);
        point p2(4,9, -16,1);
        assert(p2.length() == sqrt(4*4+9*9+16*16));
    }
}

void pointPatternClass()
{
    std::cout << "\n\n";
    std::cout << "class pointpattern: addpoint, clear()\n";
    {
        pointpattern pp;
        assert( pp.points.size() == 0);
        pp.addpoint(1, 2,3,4); 
        assert( pp.points.size() == 1);
        pp.clear();
        assert( pp.points.size() == 0);

        for (unsigned long i = 0; i != 10000; ++i)
        {
            pp.addpoint(i, i/2.0, i/3.0, i/4.0);
        }
        assert( pp.points.size() == 10000);

        pp.clear();
        assert( pp.points.size() == 0);
    }

    std::cout << "class pointpattern: addpointForCell, clear()\n";
    {
        pointpattern pp;
        assert( pp.points.size() == 0);
        pp.addpointForCell(1, 2,3,4,1,8); 
        assert( pp.points.size() == 1);
        assert(pp.points[0].faceID == 1);
        assert(pp.points[0].cellID == 8);
        pp.clear();
        assert( pp.points.size() == 0);

        for (unsigned long i = 0; i != 10000; ++i)
        {
            pp.addpointForCell(i, i/2.0, i/3.0, i/4.0, i, i*2);
        }
        assert( pp.points.size() == 10000);

        pp.clear();
        assert( pp.points.size() == 0);
    }


    std::cout << "class pointpattern: removeduplicates()\n";
    {
        {
            pointpattern pp;
            pp.addpoint(1, 2,3,4); 
            pp.addpoint(2, 2,3,4); 
            assert( pp.points.size() == 2);
            pp.removeduplicates(1e-8);
            assert( pp.points.size() == 1);

            pp.addpoint(3, 5,5,5); 
            assert( pp.points.size() == 2);
            pp.removeduplicates(1e-8);
            assert( pp.points.size() == 2);
        
        }

        {
            pointpattern pp;
            pp.addpoint(1, 2.001,3,4); 
            pp.addpoint(1, 2,3,4); 
            assert( pp.points.size() == 2);
            
            pp.removeduplicates(1e-2);
            assert( pp.points.size() == 1);
        }

        {
            pointpattern pp;
            pp.addpoint(1, -2,-3,-4); 
            pp.addpoint(2, -2,-3,-4); 
            assert( pp.points.size() == 2);
            
            pp.removeduplicates(1e-8);
            assert( pp.points.size() == 1);
        }
    }


}
