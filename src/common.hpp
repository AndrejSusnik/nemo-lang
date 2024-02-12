#pragma once
#include <iostream>
#include <map>
#include <memory>

enum class BuiltinType
{
    INT,
    CHAR,
    STRING,
    LAMBDA,
    VOID
};

class VariableValue
{
public:
    VariableValue(BuiltinType type, void *value) : type(type), value(value)
    {
    }

    BuiltinType getType()
    {
        return type;
    }

    void *getValue()
    {
        return value;
    }

private:
    BuiltinType type;
    void *value;
};

const auto converter = [](auto value) {
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
    std::map<std::string, std::string> variables;
};
