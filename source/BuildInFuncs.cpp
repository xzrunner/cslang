#include "cslang/BuildInFuncs.h"
#include "cslang/EvalAST.h"

namespace cslang
{

void SetupBuildInFuncs()
{
    RegistBuildInFunc("min", [](const std::vector<Variant>& params, const void* ud)->Variant {
        return Variant();
    });
    RegistBuildInFunc("max", [](const std::vector<Variant>& params, const void* ud)->Variant {
        return Variant();
    });
    RegistBuildInFunc("floor", [](const std::vector<Variant>& params, const void* ud)->Variant {
        if (params.empty()) {
            return Variant();
        }

        auto& p = params[0];
        if (p.type == VarType::Float) {
            return Variant(std::floor(p.f));
        } else if (p.type == VarType::Double) {
            return Variant(std::floor(p.d));
        } else {
            return p;
        }
    });
}

}