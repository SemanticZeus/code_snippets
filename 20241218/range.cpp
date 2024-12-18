#include <iostream>
#include <ranges>
#include <vector>
#include <algorithm>

int main(int argc, char *argv[])
{
  auto range = std::ranges::views::iota(0,1000000);
  std::vector<int> ans;
  std::transform(range.begin(), range.end(), std::back_inserter(ans), [](auto x) { return x*2; });
  for (auto x : ans) std::cout << x << std::endl;
  return 0;
}
