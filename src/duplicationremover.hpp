#ifndef DUPLICATIONREMOVER_H_1234567
#define DUPLICATIONREMOVER_H_1234567

#include <vector>
#include "pointpattern.hpp"

class duplicationremover
{
public:
    duplicationremover(unsigned int _x, unsigned int _y, unsigned int _z) : x(_x), y(_y), z(_z)
    {
        list.resize(x*y*z);
    };

    void addPoints ( pointpattern& porig)
    {
        std::cout << "adding N= " <<porig.points.size() <<  " points for duplication check" << std::endl;
        for (  auto it = porig.points.begin(); 
               it != porig.points.end(); 
               ++it)
        {
            addpoint(it->x, it->y, it->z, it->l);        
        }
    }

    // TODO Boundaries
    void setboundaries (double xmi, double xma, double ymi, double yma, double zmi, double zma)
    {
        xmin = xmi;
        ymin = ymi;
        zmin = zmi;
        xmax = xma;
        ymax = yma;
        zmax = zma;
    }

    void addpoint(double dx, double dy, double dz, unsigned long long l)
    {
        if (dx > xmax || dx < xmin)
        {
            std::cerr << "ignoring point with label: " << l << " due to X boundaries" << std::endl;
            return; // ignore points outside of boundaries
        }
        if (dy > ymax || dy < ymin)
        { 
            std::cerr << "ignoring point with label: " << l << " due to Y boundaries" << std::endl;
            return;
        }
        if (dz > zmax || dz < zmin) 
        {
            std::cerr << "ignoring point with label: " << l << " due to Z boundaries" << std::endl;
            return;
        }
        
        //std::cout << "adding particle " << dx << " " << dy << " "  << dz << " " << zmax  << std::endl;
        double sx = (xmax-xmin)/x;
        double sy = (ymax-ymin)/y;
        double sz = (zmax-zmin)/z;

        int cx = static_cast<int>((dx - xmin)/sx);
        int cy = static_cast<int>((dy - ymin)/sy);
        int cz = static_cast<int>((dz - zmin)/sz);
        

        // sometimes, double comparison goes wrong, therefore handle indices manually
        if (cx < 0 ) cx++;
        if (cy < 0 ) cy++;
        if (cz < 0 ) cz++;

        if (static_cast<unsigned int>(cx) >= x ) cx--;
        if (static_cast<unsigned int>(cy) >= y ) cy--;
        if (static_cast<unsigned int>(cz) >= z ) cz--;
      
        //std::cout << cx << " " << cy << " " << cz << std::endl;
        //std::cout << xmin << std::endl;

        unsigned int index = getindex(cx,cy,cz);
        //std::cout << "index: " << index << " " << list.size() <<  std::endl;
        list[index].addpoint(dx,dy,dz, l);

    }
    
    void getallPoints ( pointpattern& p)
    {
        p.points.clear();
        unsigned int i = 0;
        for (
                auto it = list.begin(); 
                it != list.end();
                ++it)
        {
            for (
                    auto itp = (*it).points.begin();
                    itp != (*it).points.end();
                    ++itp)
            {
                p.addpoint(itp->x, itp->y, itp->z, itp->l);
                i++;
            }
        }
        std::cout << "found " << i << std::endl;


    }; 



    // if indexShifts equals null, dont do any rearrangements
    void removeduplicates(double epsilon)
    {
        std::cout << "removing duplicates within one box" << std::endl;
        for (
                auto it = list.begin(); 
                it != list.end();
                ++it)
        {
            (*it).removeduplicates(epsilon);
        }
        
        std::cout << "removing duplicates between neighboring boxes" << std::endl;
        for (unsigned int cx = 0; cx != x; ++cx)    
        for (unsigned int cy = 0; cy != y; ++cy)    
        for (unsigned int cz = 0; cz != z; ++cz)
        {
            for (int i = -1; i != 1; ++i)
            for (int j = -1; j != 1; ++j)
            for (int k = -1; k != 1; ++k)
            {
                if (i == 0 && j == 0 && k == 0) continue;
                int ox = cx + i;
                int oy = cy + j;
                int oz = cz + k;
                if (ox < 0 ||static_cast<unsigned int >(ox) >= x ) continue;
                if (oy < 0 ||static_cast<unsigned int >(oy) >= y ) continue;
                if (oz < 0 ||static_cast<unsigned int >(oz) >= z ) continue;
                
                int index1 = getindex(cx,cy,cz);
                int index2 = getindex(ox,oy,oz);
                list[index1].removeduplicates(epsilon, list[index2]);
            }

        }
        
        for (
                auto it = list.begin(); 
                it != list.end();
                ++it)
        {
            
            for (auto it2 = (*it).indexShift.begin();
                    it2 != (*it).indexShift.end();
                    ++it2)
            {
                    indexShift[(*it2).first] = (*it2).second;
            }
        }
    };

    unsigned int getindex (unsigned int cx, unsigned int cy, unsigned int cz)
    {
       return cx + x * cy + x*y*cz;
    }


    void applyIndexShifts(std::map<unsigned int, std::vector<unsigned int > >& faces)
    {
        for (auto it = faces.begin(); it != faces.end(); ++ it)
        {
            for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                unsigned int vertexIndex = (*it2);
                
                while (indexShift[vertexIndex] != -1)
                {
                    vertexIndex = indexShift[vertexIndex];
                }
                (*it2) = vertexIndex;
            }
        }
    }

    std::map<unsigned int, long> indexShift;


private:
   std::vector<pointpattern> list;
    
   unsigned int x;
   unsigned int y;
   unsigned int z;

   double xmin, xmax, ymin, ymax, zmin, zmax;  
};

#endif