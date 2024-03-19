#include "Application.h"

int main(int argc, char *argv[])
{
    Application app;
    if (app.initialize(argc, argv) != AstralRaytracer::Errors::GenericError::SUCCESS)
    {
        return -1;
    }
    app.run();
    app.shutdown();
    return 0;
}