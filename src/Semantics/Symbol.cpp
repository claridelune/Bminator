#include "Symbol.h"
#include <iostream>

void BuiltInSymbol::Render() const {
    std::cout << "Built-In Symbol: " << name << "\n";
}

void VariableSymbol::Render() const {
    std::cout << "Variable Symbol: " << name << ", Type: " << (type ? type->GetName() : "None") 
              << ", Offset: " << offset << "\n";
}

void NestedScopeSymbol::Render() const {
    std::cout << "Nested Scope Symbol: " << name << "\n";
}

void FunctionSymbol::Render() const {
    std::cout << "Function Symbol: " << name << ", Return Type: " << type->GetName() << std::endl;
}

void FunctionSymbol::AddParameterType(const Symbol* type) {
    parameterTypes.push_back(type);
}

const std::vector<const Symbol*>& FunctionSymbol::GetParameterTypes() const {
    return parameterTypes;
}

SymbolTable::SymbolTable(const std::string& name, int level, SymbolTable* parent)
    : scopeName(name), scopeLevel(level), parentScope(parent) {}

bool SymbolTable::DefineSymbol(std::unique_ptr<Symbol> symbol) {
    const std::string& name = symbol->GetName();
    auto result = symbols.insert({name, std::move(symbol)});
    return result.second;
}

const Symbol* SymbolTable::LookUpSymbol(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    return parentScope ? parentScope->LookUpSymbol(name) : nullptr;
}

int SymbolTable::AllocateOffset() {
    static int offsetCounter = 0;
    offsetCounter -= 4;
    return offsetCounter;
}

void SymbolTable::Render() const {
    std::cout << "Symbol Table: " << scopeName << " (Level " << scopeLevel << ")\n";
    for (const auto& entry : symbols) {
        entry.second->Render();
    }
    std::cout << "\n";
}
