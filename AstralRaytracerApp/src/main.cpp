#include "Application.h"

int main()
{
    Application::LaunchOptions options;
    options.logLevel = Application::LogLevel::Info;
    options.logDisplay = Application::LogDisplay::Console;

    Application app(options);
    app.initialize();
    app.run();
    app.shutdown();
    return 0;
}