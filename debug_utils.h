#include <iostream>
#define deb(x) std::cout << #x << " : " << x << std::endl

#define prompt(x) do { \
  std::string _; \
  std::cout << (x) << std::endl; \
  std::cin >> _; \
} while(false);

template<typename... Args>
struct Typer;
