#include "token.hpp"

#include <random>

std::string GenerateToken() {
  static const std::string alpha_numeric =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  static std::random_device rd;
  static std::mt19937 gen{rd()};
  static std::uniform_int_distribution<int> distrib(0,
                                                    alpha_numeric.size() - 1);
  const size_t token_len = 20;
  std::string token(token_len, ' ');
  for (char& c : token) {
    c = alpha_numeric[distrib(gen)];
  }
  return token;
}