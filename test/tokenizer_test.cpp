#include "utility.h"

#include <cslang/Tokenizer.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("hello world")
{
    auto str = R"(
int main()
{
    printf("hello.");
    return 0;
}
)";

    cslang::Tokenizer tokenizer(str);
//    test::debug_print(tokenizer);
}