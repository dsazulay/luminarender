#include "application.h"

int main()
{
    AppConfig config{};
    config.windowName = "Interactive Graphics";
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.viewportWidth = 640;
    config.viewportHeight = 480;

    Application app(config);
    app.run();

    return 0;
}