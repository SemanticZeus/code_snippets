#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <iostream>

using namespace boost::multiprecision;
typedef number<cpp_dec_float<100> > cpp_dec_float_100;

int main(int argc, char *argv[])
{
  double s = 0.0f;
  for (ssize_t i=100;i<999;i++) {
    double b = double(i);
    s+=1/(b*b);
  }
  std::cout << std::setprecision(100) <<"s = " << s << ",  9000*s = "  << s*9000 << std::endl;
  return 0;
}
