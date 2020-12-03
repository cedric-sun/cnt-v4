// cesun, 11/28/20 1:32 AM.

#ifndef CNT5106_V4_MATHUTILS_HPP
#define CNT5106_V4_MATHUTILS_HPP

#include <random>

class MathUtils {
private:
    static std::default_random_engine rng;
    static std::uniform_int_distribution<> uniform;
public:
    // return a uniform random integer in [0,upper)
    // precondition: upper >= 1
    static int randomInt(const int upper) {
        return uniform(rng) % upper;
    }

    static int ceilingDiv(const int64_t x, const int64_t y) {
        return static_cast<int>((x + y - 1) / y);
    }
};


#endif //CNT5106_V4_MATHUTILS_HPP
