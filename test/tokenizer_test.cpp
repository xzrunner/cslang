#include "utility.h"

#include <vexc/Tokenizer.h>

#include <catch/catch.hpp>

TEST_CASE("hello world")
{
    auto str = R"(
int main()
{
    printf("hello.");
    return 0;
}
)";

    vexc::Tokenizer tokenizer(str);
//    test::debug_print(tokenizer);
}