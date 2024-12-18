#include <iostream>
#include <boost/math/tools/roots.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/math/special_functions/sin_pi.hpp>
#include <cmath>
#include <limits>

using double50 = boost::multiprecision::cpp_dec_float_50;

int main(int argc, char *argv[])
{
  auto f = [](double50 x) {return boost::multiprecision::sin(x)*boost::multiprecision::cos(9*x)+3*boost::math::pow<2>(x)+34+4*boost::math::pow<3>(x);};
  std::uintmax_t iter = 100;
  auto root = boost::math::tools::newton_raphson_iterate (f, (double50)0.0, (double50)-1000, (double50)1000, 10);
  std::cout << root << std::endl;
  return 0;
}
