#include <random>

class RNG
{
  public:
    std::random_device rd;
    RNG(){}
    ~RNG(){}
    int getInt(int min, int max){
    std::uniform_int_distribution<int> dist(min, max);
      return dist(rd);}
};
