#include "application.h"

int main()
{
    AppConfig config{};
    config.windowName = "Interactive Graphics";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.viewportWidth = 960;
    config.viewportHeight = 540;

    Application app(config);
    app.run();

    return 0;
}
