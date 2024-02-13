#pragma once
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mpc/mpc.h>
#include <optional>
#include <stdexcept>
#include <variant>
#include <vector>

enum class BuiltinType { INT, CHAR, STRING, COLLECTION, LAMBDA, VOID };

using NemoValue = std::variant<int, char, std::string, const mpc_ast_t *>;

struct NemoType {
  BuiltinType type;
  std::optional<NemoValue> value;
  std::optional<std::vector<NemoType>> collection;

  void print() const {

    switch (type) {
    case BuiltinType::VOID:
      std::cout << "None";
      break;
    case BuiltinType::INT:
      std::cout << std::get<int>(value.value_or(0));
      break;
    case BuiltinType::CHAR:
      std::cout << std::get<char>(value.value_or(0));
      break;
    case BuiltinType::STRING:
      std::cout << std::get<std::string>(value.value_or(""));
      break;
    case BuiltinType::COLLECTION:
      std::cout << "[ ";
      for (const auto &item : collection.value_or(std::vector<NemoType>{})) {
        item.print();
        std::cout << " ";
      }
      std::cout << "]";
      break;
    default:
      std::cout << "Not implemeneted";
    }
  }

  void debugPrint() const {}
};

NemoType voidType() {
  NemoType type;
  type.type = BuiltinType::VOID;

  return type;
}

NemoType numberType(int value) {
  NemoType type;
  type.type = BuiltinType::INT;
  type.value = value;

  return type;
}

NemoType stringType(std::string value) {
  NemoType type;
  type.type = BuiltinType::STRING;
  type.value = value;

  return type;
}

NemoType charType(char value) {
  NemoType type;
  type.type = BuiltinType::CHAR;
  type.value = value;

  return type;
}

NemoType lambdaType(const mpc_ast_t *value) {
  NemoType type;
  type.type = BuiltinType::LAMBDA;
  type.value = value;

  return type;
}

NemoType collectionType(std::vector<NemoType> value) {
  NemoType type;
  type.type = BuiltinType::COLLECTION;
  type.collection = value;

  return type;
}

class ScopeContext {
public:
  ScopeContext(std::shared_ptr<ScopeContext *> parent = nullptr)
      : parent(parent) {}

  std::weak_ptr<ScopeContext *> createChildContext(std::string name) {
    std::shared_ptr<ScopeContext *> child =
        std::make_shared<ScopeContext *>(this);
    children[name] = child;
    return child;
  }

  void registerFunction(std::string name,
                        std::function<NemoType(std::vector<NemoType>)> func) {
    if (functions.find(name) == functions.end()) {
      functions.insert({name, func});
    } else {
      functions[name] = func;
    }
  }

  void bind(std::string name, NemoType type) {
    if (variables.find(name) == variables.end()) {
      variables.insert({name, type});
    } else {
      variables[name] = type;
    }
  }

  int size() { return variables.size(); }

  NemoType get(std::string name) {
    if (variables.find(name) != variables.end()) {
      return variables[name];
    } else {
      if (parent != nullptr) {
        return (*parent)->get(name);
      } else {
        throw std::invalid_argument("Variable not found");
      }
    }
  }

  NemoType callFunction(std::string name, std::vector<NemoType> args) {
    if (functions.find(name) != functions.end()) {
      return functions[name](args);
    } else {
      if (parent != nullptr) {
        return (*parent)->callFunction(name, args);
      } else {
        throw std::invalid_argument("Function not found");
      }
    }
  }

private:
  std::shared_ptr<ScopeContext *> parent = nullptr;
  std::map<std::string, std::shared_ptr<ScopeContext *>> children;
  std::map<std::string, NemoType> variables;
  std::map<std::string, std::function<NemoType(std::vector<NemoType>)>>
      functions;
};
