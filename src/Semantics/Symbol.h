#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <map>
#include <memory>
#include <vector>

class Symbol {
protected:
    std::string name;
    std::string offset;
    friend class SymbolTable;
public:
    const Symbol* type;
    Symbol(const std::string& n, const std::string& off = "0", const Symbol* t = nullptr) 
        : name(n), offset(off), type(t) {}
    virtual ~Symbol() = default;

    std::string GetName() const { return name; }
    virtual void Render() const = 0;
    virtual bool IsCompatibleWith(const Symbol* other) const = 0;
};

class BuiltInSymbol : public Symbol {
public:
    BuiltInSymbol(const std::string& n) : Symbol(n) {}
    ~BuiltInSymbol() override = default;

    void Render() const override;
    bool IsCompatibleWith(const Symbol* other) const override {
        const auto* otherBuiltIn = dynamic_cast<const BuiltInSymbol*>(other);
        return otherBuiltIn && this->name == otherBuiltIn->name;
    }

};

class VariableSymbol : public Symbol {
public:
    VariableSymbol(const std::string& n, const std::string& off, const Symbol* t) 
        : Symbol(n, off, t) {}
    ~VariableSymbol() override = default;

    bool IsCompatibleWith(const Symbol* other) const override {
        return this->type->IsCompatibleWith(other);
    }

    void Render() const override;
};

class NestedScopeSymbol : public Symbol {
public:
    NestedScopeSymbol(const std::string& n) : Symbol(n) {}
    ~NestedScopeSymbol() override = default;

    void Render() const override;
};

class SymbolTable {
private:
    std::map<std::string, std::unique_ptr<Symbol>> symbols;
    std::string scopeName;
    int scopeLevel;
    SymbolTable* parentScope;

    const Symbol* returnType;

public:
    SymbolTable(const std::string& name, int level, SymbolTable* parent = nullptr);

    bool DefineSymbol(std::unique_ptr<Symbol> symbol);
    const Symbol* LookUpSymbol(const std::string& name) const;
    int GetScopeLevel() const { return scopeLevel; }
    std::string GetScopeName() const { return scopeName; }
    SymbolTable* GetParentScope() const { return parentScope; }
    void SetReturnType(const Symbol* type) { returnType = type; }
    const Symbol* GetReturnType() const { return returnType; }
    int AllocateOffset(); // Manage memory offsets for variables

    void Render() const;
};

#endif
