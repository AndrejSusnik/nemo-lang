#include "interpreter/interpreter.h"
#include "mpc/mpc.h"
#include "nemo/common.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

void registerBuiltinFunctions(std::shared_ptr<ScopeContext> ctx);
void eval(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx);
void eval_assignment(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx);
NemoType eval_pipeline(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx);
NemoType eval_expression(const mpc_ast_t *ast,
                         std::shared_ptr<ScopeContext> ctx,
                         std::vector<NemoType> args);

bool evaluate(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx) {

  registerBuiltinFunctions(ctx);

  for (int i = 0; i < ast->children_num; i++) {
    eval(ast->children[i], ctx);
  }

  return true;
}

std::string typeToString(BuiltinType type) {
  switch (type) {
  case BuiltinType::INT:
    return "int";
  case BuiltinType::CHAR:
    return "char";
  case BuiltinType::STRING:
    return "string";
  case BuiltinType::LAMBDA:
    return "lambda";
  case BuiltinType::VOID:
    return "void";
  default:
    return "unknown";
  }
}

void registerBuiltinFunctions(std::shared_ptr<ScopeContext> ctx) {
  ctx->registerFunction("print", [](std::vector<NemoType> args) {
    for (const auto arg : args) {
      arg.print();
    }

    return voidType();
  });

  ctx->registerFunction("println", [](std::vector<NemoType> args) {
    for (const auto arg : args) {
      arg.print();
    }
    std::cout << std::endl;
    return voidType();
  });

  ctx->registerFunction("exit", [](std::vector<NemoType> args) {
    if (args.size() != 1) {
      throw std::runtime_error("exit function takes exactly one argument");
    }

    const auto &arg = args[0];

    if (arg.type != BuiltinType::INT) {
      throw std::runtime_error(
          "exit function takes an integer argument, but got " +
          typeToString(arg.type));
    }

    const auto exitCode = std::get<int>(arg.value.value());
    exit(exitCode);
    return voidType();
  });
}

void eval(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx) {
  const auto tag = std::string(ast->tag);
  if (tag.find("assignment") != std::string::npos) {
    eval_assignment(ast, ctx);
  }
  if (tag.find("pipeline") != std::string::npos) {
    eval_pipeline(ast, ctx);
  }
}

void eval_assignment(const mpc_ast_t *ast, std::shared_ptr<ScopeContext> ctx) {
  const auto bindType = std::string(ast->children[0]->contents);
  const auto identifier = std::string(ast->children[1]->contents);

  const auto pipelineResult = eval_pipeline(ast->children[3], ctx);

  ctx->bind(bindType, identifier, pipelineResult);
}

NemoType eval_pipeline(const mpc_ast_t *ast,
                       std::shared_ptr<ScopeContext> ctx) {
  const auto tag = std::string(ast->tag);
  NemoType result;

    mpc_ast_print(const_cast<mpc_ast_t*>(ast));

  result = [&]() {
    if (ast->children_num == 0 ||
        std::string(ast->tag).find("lambda") != std::string::npos) {
      return eval_expression(ast, ctx, std::vector<NemoType>());
    } else {
      return eval_expression(ast->children[0], ctx, std::vector<NemoType>());
    }
  }();

  const int startIdx = [&]() {
    if (result.type == BuiltinType::LAMBDA) {
        return 1;
    } else {
      return 1;
    }
  }();

  for (int i = startIdx; i < ast->children_num; i++) {
    if (std::string(ast->children[i]->tag) == "string")
      continue;

    const auto child = ast->children[i];
    result = eval_expression(child, ctx, std::vector<NemoType>{result});
  }

  return result;
}

NemoType eval_expression(const mpc_ast_t *ast,
                         std::shared_ptr<ScopeContext> ctx,
                         std::vector<NemoType> args) {
  if (args.size() == 0) {

    const auto tag = std::string(ast->tag);

    if (tag.find("ident") != std::string::npos) {
      try {
        return ctx->get(std::string(ast->contents));
      } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return voidType();
      }
    } else if (tag.find("number") != std::string::npos) {
      return numberType(std::stoi(ast->contents));
    } else if (tag.find("string") != std::string::npos) {
      return stringType(std::string(ast->contents));
    } else if (tag.find("character") != std::string::npos) {
      return charType(ast->contents[0]);
    } else if (tag.find("lambda") != std::string::npos) {
      return lambdaType(ast);
    } else {
      std::cout << "Not implemented" << std::endl;
      return voidType();
    }
  } else {
    try {

      return ctx->callFunction(std::string(ast->contents), args);
    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
      return voidType();
    }
  }
}