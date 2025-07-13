#include <ctime>

class Timer {
public:
    static void update();

    static float time;
    static float deltaTime;
    static float lastFrameTime;
    static std::time_t systemTime;
};
