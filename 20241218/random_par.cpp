#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <execution>
#include <ranges>

std::ostream& operator<<(std::ostream& os, const std::vector<std::vector<char>>& v)
{
  for (const auto &r : v) {
    for (auto c : r) os << c;
    os << std::endl;
  }
  return os;
}

int main(int argc, char *argv[])
{
  int range = 10000;
  std::random_device rd;
  int mean = 10;
  int var = 500;
  std::normal_distribution<double> nd(mean,var);
  std::vector<int> hist(range,0);
  std::mt19937 e{rd()};
  constexpr size_t experiments = 1000000000;
  std::vector<size_t> values(100, 1);
  for (int i=0;i<experiments/100;i++)
  std::for_each(std::execution::par, values.begin(), values.end(),[&](int i) {
      double x = (nd(e)-mean)+range/2;
      if (x<0 || x>=range) return;
      hist[(int)x]++;
      });
  int m = *std::max_element(hist.begin(), hist.end());
  int num_rows = 50;
  std::vector<std::vector<char>> plot(num_rows, std::vector<char>(range, ' '));
  for (auto i=0;i<range;i++) {
    int l = hist[i]*num_rows/m;
    for (int j=0;j<l;j++) {
      plot[num_rows-j-1][i]='*';
    }
  }
  std::cout << plot << std::endl;
  return 0;
}
