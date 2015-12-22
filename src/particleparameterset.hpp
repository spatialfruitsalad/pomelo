#ifndef PARTICLEPARAMETER_GUARD
#define PARTICLEPARAMETER_GUARD

#include <vector>

class particleparameterset
{
public:
    std::vector<double> parameter;
    void push_back(double x)
    {
        parameter.push_back(x);
    };
    double get(int index)
    {
        double v = parameter.at(index);
        return v;
    };
};


#endif
