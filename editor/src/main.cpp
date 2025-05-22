#include "application.h"

int main() {
  AppConfig config{
      .windowName = "Lumina",
      .windowWidth = 1280,
      .windowHeight = 720,
      .viewportWidth = 960,
      .viewportHeight = 540,
  };

  Application app(config);
  app.run();

  return 0;
}
