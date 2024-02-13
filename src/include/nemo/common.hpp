#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <variant>
#include <vector>
#include <mpc/mpc.h>
#include <functional>
#include <optional>

enum class BuiltinType
{
    INT,
    CHAR,
    STRING,
    LAMBDA,
    VOID
};


using NemoValue = std::variant<int, char, std::string_view, const mpc_ast_t*>;

struct NemoType
{
    BuiltinType type;
    std::optional<NemoValue> value;

    void print() const {

        switch (type)
        {
        case BuiltinType::VOID:
            std::cout << "None" << std::endl;
        break;
        case BuiltinType::INT:
            std::cout << std::get<int>(value.value_or(0)) << std::endl;
            break;
        default:
            std::cout << "Not implemeneted" << std::endl;
        }
    }

    void debugPrint() const {

    }
};

NemoType voidType() {
    NemoType type;
    type.type = BuiltinType::VOID;

    return type;
}


class ScopeContext
{
public:
    ScopeContext(std::shared_ptr<ScopeContext *> parent = nullptr) : parent(parent)
    {
    }

    std::weak_ptr<ScopeContext *> createChildContext(std::string name)
    {
        std::shared_ptr<ScopeContext *> child = std::make_shared<ScopeContext *>(this);
        children[name] = child;
        return child;
    }

    void registerFunction(std::string name, std::function<NemoType(std::vector<NemoType>)> func) {
        if (functions.find(name) == functions.end()) {
            functions.insert({name, func});
        }
        else {
            functions[name] = func;
        }
    }

private:
    std::shared_ptr<ScopeContext *> parent = nullptr;
    std::map<std::string, std::shared_ptr<ScopeContext *>> children;
    std::map<std::string, NemoType> variables;
    std::map<std::string, std::function<NemoType(std::vector<NemoType>)>> functions;
};
