// cesun, 11/28/20 1:32 AM.

#include "MathUtils.hpp"

std::default_random_engine MathUtils::rng{std::random_device{}()};
std::uniform_int_distribution<> MathUtils::uniform{};
