#include "interpreter/interpreter.h"
#include "mpc/mpc.h"
#include "nemo/common.hpp"

#include <vector>
#include <memory>
#include <iostream>

void registerBuiltinFunctions(std::shared_ptr<ScopeContext> ctx);
void eval(const mpc_ast_t* ast);

bool evaluate(const mpc_ast_t* ast) {

    std::shared_ptr<ScopeContext> globalContext = std::make_shared<ScopeContext>();
    registerBuiltinFunctions(globalContext);

    eval(ast);

    return true;
}

void registerBuiltinFunctions(std::shared_ptr<ScopeContext> ctx) {
    ctx->registerFunction("print", [](std::vector<NemoType> args) {
        for (const auto arg: args) {
            arg.print();
        }

        return voidType();
    });
}

void eval(const mpc_ast_t* ast) {
    std::cout << ast->tag << std::endl;
}
