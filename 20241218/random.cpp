#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <execution>
#include <ranges>
#include <chrono>

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
  auto start_time = std::chrono::system_clock::now();
  int range = 10000;
  std::random_device rd;
  int mean = 10;
  int var = 500;
  std::normal_distribution<double> nd(mean,var);
  std::vector<int> hist(range,0);
  std::mt19937 e{rd()};
  constexpr size_t experiments = 1000000000;
  for (size_t i=0;i<experiments;i++) {
      double x = (nd(e)-mean)+range/2;
      if (x<0 || x>=range) continue;
      hist[(int)x]++;
  };
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
  auto end_time = std::chrono::system_clock::now();
  std::cout << "duration = " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time) << std::endl;
  return 0;
}
