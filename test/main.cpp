#include <vexc/Type.h>
#include <vexc/BuildInFuncs.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

int main(int argc, char* argv[])
{
    vexc::SetupTypeSystem();
    vexc::SetupBuildInFuncs();

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
}