#pragma once

#include "mpc/mpc.h"
#include "nemo/common.hpp"

bool evaluate(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx);