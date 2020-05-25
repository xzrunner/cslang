#include "utility.h"

#include <cslang/Tokenizer.h>

namespace test
{

void debug_print(cslang::Tokenizer& tk)
{
    tk.Reset();

    while (true)
    {
        auto token = tk.NextToken();
        auto token_type = token.GetType();
        if (token_type == cslang::TK_END) {
            break;
        }
        else {
            printf("%d %10s %10.*s\n",
                token_type,
                cslang::Tokenizer::TokenStrings[token_type],
                token.End() - token.Begin(), token.Begin());
        }
    }
}

}