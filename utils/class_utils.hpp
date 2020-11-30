// cesun, 11/23/20 3:30 PM.

#ifndef CNT5106_V4_CLASS_UTILS_HPP
#define CNT5106_V4_CLASS_UTILS_HPP

#define DISABLE_COPY(CLASSNAME) CLASSNAME(CLASSNAME &) = delete;\
    void operator=(CLASSNAME &) = delete;

#define DISABLE_MOVE(CLASSNAME) CLASSNAME(CLASSNAME &&) = delete; \
    void operator=(CLASSNAME &&) = delete;

#define DISABLE_COPY_MOVE(CLASSNAME) DISABLE_COPY(CLASSNAME) DISABLE_MOVE(CLASSNAME)

#define DEFAULT_MOVE(CLASSNAME) CLASSNAME(CLASSNAME &&) = default; \
    CLASSNAME &operator=(CLASSNAME &&) = default;

#define DFT_MOVE_CTOR_ONLY(CLASSNAME) DISABLE_COPY(CLASSNAME) \
CLASSNAME(CLASSNAME &&) = default;                            \
    void operator=(CLASSNAME &&) = delete;
#endif //CNT5106_V4_CLASS_UTILS_HPP
