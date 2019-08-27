#include "utility.h"

#include <vexc/Tokenizer.h>

namespace test
{

void debug_print(vexc::Tokenizer& tk)
{
    tk.Reset();

    while (true)
    {
        auto token = tk.NextToken();
        auto token_type = token.GetType();
        if (token_type == vexc::TK_END) {
            break;
        }
        else {
            printf("%d %10s %10.*s\n",
                token_type,
                vexc::Tokenizer::TokenStrings[token_type],
                token.End() - token.Begin(), token.Begin());
        }
    }
}

}