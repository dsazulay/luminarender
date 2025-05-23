#include <ctime>

class Timer {
public:
    static void update();

    static float time;
    static std::time_t systemTime;
};
