#include<fstream>
#include<iostream>

#include<sstream>
#include<string>
#include "fileloader.hpp"

void fileloader::read(std::string filename, std::vector<particleparameterset>& set)
{
    std::ifstream infile;
    infile.open(filename, std::ifstream::in);
    if (infile.fail())
    {
        std::cout << "Cannot load file " << filename << std::endl;
        return;
    }
    std::string line;
    unsigned int linesloaded = 0;
    std::getline(infile, line);
    while (std::getline(infile, line))
    {
        if(line.find("#")!=std::string::npos) continue; // ignore comment lines

        std::istringstream iss(line);
        double l, x, y, z, r;
        if (!(iss >> l >> x >> y >> z >> r))
        {
            break;    // error
        }

        particleparameterset p;
        p.push_back(l);
        p.push_back(x);
        p.push_back(y);
        p.push_back(z);
        p.push_back(r);
        linesloaded++;
        set.push_back(p);

    }
    std::cout << "Lines loaded: " << linesloaded << std::endl << std::endl;

    infile.close();
}
