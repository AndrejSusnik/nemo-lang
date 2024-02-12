#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <variant>
#include <vector>
#include <mpc/mpc.h>
#include <functional>

enum class BuiltinType
{
    INT,
    CHAR,
    STRING,
    LAMBDA,
    VOID
};


using NemoValue = std::variant<int, char, std::string_view, std::weak_ptr<mpc_ast_t*>>;

struct NemoType
{
    BuiltinType type;
    NemoValue value;
};

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

private:
    std::shared_ptr<ScopeContext *> parent = nullptr;
    std::map<std::string, std::shared_ptr<ScopeContext *>> children;
    std::map<std::string, NemoType> variables;
};
