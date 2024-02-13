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

NemoType eval_operator(const NemoType &op1, const NemoType &op2,
                       const std::string &op);

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
  case BuiltinType::COLLECTION:
    return "collection";
  case BuiltinType::VOID:
    return "void";
  default:
    return "unknown";
  }
}

void registerBuiltinFunctions(std::shared_ptr<ScopeContext> ctx) {
  ctx->registerFunction("print", [](std::vector<NemoType> args) {
    for (const auto &arg : args) {
      arg.print();
    }

    return voidType();
  });

  ctx->registerFunction("println", [](std::vector<NemoType> args) {
    for (const auto &arg : args) {
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

  ctx->registerFunction("len", [](std::vector<NemoType> args) {
    if (args.size() != 1) {
      throw std::runtime_error("len function takes exactly one argument");
    }

    const auto &arg = args[0];

    if (arg.type != BuiltinType::COLLECTION &&
        arg.type != BuiltinType::STRING) {
      throw std::runtime_error(
          "len function takes a collection or string argument, but got " +
          typeToString(arg.type));
    }

    return [&]() {
      if (arg.type == BuiltinType::COLLECTION) {
        return numberType(arg.collection.value().size());
      } else {
        return numberType(std::get<std::string>(arg.value.value()).size());
      }
    }();
  });

  ctx->registerFunction("sum", [](std::vector<NemoType> args) {
    if (args.size() != 1) {
      throw std::runtime_error("sum function takes exactly one argument");
    }

    if (args[0].type != BuiltinType::COLLECTION) {
      throw std::runtime_error(
          "sum function takes a collection argument, but got " +
          typeToString(args[0].type));
    }

    int partialSum = 0;
    for (const auto &arg : args[0].collection.value()) {
      if (arg.type != BuiltinType::INT) {
        voidType();
      }

      partialSum += std::get<int>(arg.value.value());
    }

    return numberType(partialSum);
  });

  ctx->registerFunction("to_string", [](std::vector<NemoType> args) {
    if (args.size() != 1) {
      throw std::runtime_error("to_string function takes exactly one argument");
    }

    const auto &arg = args[0];

    switch (arg.type) {
    case BuiltinType::INT:
      return stringType(std::to_string(std::get<int>(arg.value.value())));
    case BuiltinType::CHAR:
      return stringType(std::string(1, std::get<char>(arg.value.value())));
    case BuiltinType::STRING:
      return arg;
    default:
      return voidType();
    }
  });

  ctx->registerFunction("join", [](std::vector<NemoType> args) {
    // joins collection of characters into a string
    if (args.size() != 1) {
      throw std::runtime_error("join function takes exactly one argument");
    }

    if (args[0].type != BuiltinType::COLLECTION) {
      throw std::runtime_error(
          "join function takes a collection argument, but got " +
          typeToString(args[0].type));
    }

    std::string result;
    for (const auto &arg : args[0].collection.value()) {
      if (arg.type != BuiltinType::CHAR) {
        voidType();
      }

      result += std::get<char>(arg.value.value());
    }

    return stringType(result);
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
  // const auto bindType = std::string(ast->children[0]->contents);
  const auto identifier = std::string(ast->children[1]->contents);

  const auto pipelineResult = eval_pipeline(ast->children[3], ctx);

  ctx->bind(identifier, pipelineResult);
}

NemoType eval_pipeline(const mpc_ast_t *ast,
                       std::shared_ptr<ScopeContext> ctx) {
  const auto tag = std::string(ast->tag);

  //   mpc_ast_print(const_cast<mpc_ast_t *>(ast));

  if (tag.find("expression") != std::string::npos) {
    return eval_expression(ast, ctx, std::vector<NemoType>());
  } else {
    NemoType result = voidType();

    for (int i = 0; i < ast->children_num; ++i) {
      const auto args = [&]() {
        if (i == 0)
          return std::vector<NemoType>();
        else
          return std::vector<NemoType>({result});
      }();

      if (std::string(ast->children[i]->tag).find("operator") !=
          std::string::npos) {
        NemoType nextOp =
            eval_expression(ast->children[++i], ctx, std::vector<NemoType>());
        result = eval_operator(result, nextOp,
                               std::string(ast->children[i - 1]->contents));
      } else {

        if (std::string(ast->children[i]->tag).find("string") !=
            std::string::npos)
          continue;
        result = eval_expression(ast->children[i], ctx, args);
      }
    }

    return result;
  }
}

NemoType eval_expression(const mpc_ast_t *ast,
                         std::shared_ptr<ScopeContext> ctx,
                         std::vector<NemoType> args) {
  if (args.size() == 0) {

    const auto tag = std::string(ast->tag);

    if (tag.find("ident") != std::string::npos) {
      try {
        return ctx->callFunction(std::string(ast->contents), args);
      } catch (const std::exception &e) {
        try {
          return ctx->get(std::string(ast->contents));
        } catch (const std::exception &e) {
          std::cout << e.what() << std::endl;
          return voidType();
        }
      }
    } else if (tag.find("number") != std::string::npos) {
      return numberType(std::stoi(ast->contents));
    } else if (tag.find("str") != std::string::npos) {
      const auto tmp = std::string(ast->contents);
      return stringType(tmp.substr(1, tmp.length() - 2));
    } else if (tag.find("character") != std::string::npos) {
      return charType(ast->contents[1]);
    } else if (tag.find("collection") != std::string::npos) {
      std::vector<NemoType> collection;
      for (int i = 1; i < ast->children_num - 1; i++) {
        collection.push_back(
            eval_expression(ast->children[i], ctx, std::vector<NemoType>()));
      }

      return collectionType(collection);

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

NemoType eval_operator(const NemoType &op1, const NemoType &op2,
                       const std::string &op) {
  if (op1.type != op2.type) {
    std::cout << "Operator types should be equal but got types" +
                     typeToString(op1.type) + " and " + typeToString(op2.type)
              << std::endl;
    exit(0);
  }

  switch (op1.type) {
  case BuiltinType::INT: {
    if (op.compare("+") == 0) {
      return numberType(std::get<int>(op1.value.value()) +
                        std::get<int>(op2.value.value()));
    } else if (op.compare("-") == 0) {
      return numberType(std::get<int>(op1.value.value()) -
                        std::get<int>(op2.value.value()));
    } else if (op.compare("*") == 0) {
      return numberType(std::get<int>(op1.value.value()) *
                        std::get<int>(op2.value.value()));
    } else if (op.compare("/") == 0) {
      return numberType(std::get<int>(op1.value.value()) /
                        std::get<int>(op2.value.value()));
    }

  } break;
  case BuiltinType::CHAR: {
    if (op.compare("+") == 0) {
      return charType(std::get<char>(op1.value.value()) +
                      std::get<char>(op2.value.value()));
    } else if (op.compare("-") == 0) {
      return charType(std::get<char>(op1.value.value()) -
                      std::get<char>(op2.value.value()));
    } else if (op.compare("*") == 0) {
      return charType(std::get<char>(op1.value.value()) *
                      std::get<char>(op2.value.value()));
    } else if (op.compare("/") == 0) {
      return charType(std::get<char>(op1.value.value()) /
                      std::get<char>(op2.value.value()));
    }

  } break;
  case BuiltinType::STRING: {
    if (op.compare("+") == 0) {
      return stringType(std::get<std::string>(op1.value.value()) +
                        std::get<std::string>(op2.value.value()));
    } else {
    }

  } break;
  case BuiltinType::LAMBDA: {
    return voidType();

  } break;
  case BuiltinType::VOID: {
    return voidType();
  } break;
  }
}
