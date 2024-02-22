#pragma once
#include "mpc/mpc.h"

#include <string>
#include <variant>
#include <memory>
#include <vector>

namespace nemo::ir {

enum class BuiltinType {
  Number,
  Character,
  String,
  Collection,
  Lambda,
  Custom,
  Void,
  Any,
};

// Forward declarations
struct Statement;


// Base class for all primitives
struct PrimitiveBase {
  BuiltinType type;

  virtual std::string to_string() const;
};

struct Identifier: public PrimitiveBase {
  std::string name;

  std::string to_string() const override {
    return name;
  }
};

struct Number: public PrimitiveBase {
  int value;

  std::string to_string() const override {
    return "number: " + std::to_string(value);
  }
};

struct Character: public PrimitiveBase {
  char value;

  std::string to_string() const override {
    return "character: " + std::string(1, value);
  }
};

struct String: public PrimitiveBase {
  std::string value;

  std::string to_string() const override {
    return "string: " + value;
  }
};

struct Collection: public PrimitiveBase {
  BuiltinType elementType;
  std::vector<PrimitiveBase> elements;

  std::string to_string() const override {
    std::string result = "collection: [";
    for (const auto& element : elements) {
      result += element.to_string() + ", ";
    }
    result.pop_back();
    result.pop_back();
    result += "]";
    return result;
  }
};

struct Lambda: public PrimitiveBase {
  std::vector<Identifier> parameters;
  BuiltinType returnType;
  std::vector<Statement> body;

  std::string to_string() const override {
    std::string result = "lambda";
    return result;
  }
};

struct Operator {
  std::string symbol;
};

struct FunctionCall {
  Identifier function;
  std::vector<PrimitiveBase> arguments;
};

struct Pipeline {
  std::vector<std::variant<PrimitiveBase, Operator, std::string, FunctionCall>> elements;

  std::string to_string() const {
    std::string result;
    for (const auto& element : elements) {
      if (std::holds_alternative<PrimitiveBase>(element)) {
        result += std::get<PrimitiveBase>(element).to_string();
      } else if (std::holds_alternative<Operator>(element)) {
        result += std::get<Operator>(element).symbol;
      } else if (std::holds_alternative<std::string>(element)) {
        result += std::get<std::string>(element);
      } else {
        result += std::get<FunctionCall>(element).function.name + "(";
        for (const auto& argument : std::get<FunctionCall>(element).arguments) {
          result += argument.to_string() + ", ";
        }
        result.pop_back();
        result.pop_back();
        result += ")";
      }
    }
    return result;
  
  }
};

enum class AssignmentType {
  Const,
  Let,
  Var,
};

struct Assignment {
  AssignmentType type;
  Identifier variable;
  std::variant<PrimitiveBase, Pipeline> value;

  std::string to_string() const {
    std::string result;
    switch (type) {
      case AssignmentType::Const:
        result += "const ";
        break;
      case AssignmentType::Let:
        result += "let ";
        break;
      case AssignmentType::Var:
        result += "var ";
        break;
    }
    result += variable.name + " = ";
    if (std::holds_alternative<PrimitiveBase>(value)) {
      result += std::get<PrimitiveBase>(value).to_string();
    } else {
      result += std::get<Pipeline>(value).to_string();
    }
    return result;
  }
};

struct Statement {
  std::variant<Assignment, Pipeline> statement;
  std::string to_string() const {
    if (std::holds_alternative<Assignment>(statement)) {
      return std::get<Assignment>(statement).to_string();
    }
    return std::get<Pipeline>(statement).to_string();
  }
};  

struct Program {
  std::vector<Statement> statements;

  std::string to_string() const {
    std::string result;
    for (const auto& statement : statements) {
      result += statement.to_string();
    }
    return result;
  
  }
};

class Parser {
public:
  Parser() = default;

  std::unique_ptr<Program> parse(mpc_ast_t* ast) {
    return nullptr;
  }
};
}