#include "SemanticAnalyzer.h"
#include "Symbol.h"
#include "../utils/logger.h"

SemanticAnalyzer::SemanticAnalyzer()
    : currentScope(nullptr), hasError(false) {
    
    // tabla global
    auto globalScope = std::make_unique<SymbolTable>("GLOBAL", 0);
    currentScope = globalScope.get();

    currentScope->DefineSymbol(std::make_unique<BuiltInSymbol>("char"));
    currentScope->DefineSymbol(std::make_unique<BuiltInSymbol>("integer"));
    currentScope->DefineSymbol(std::make_unique<BuiltInSymbol>("boolean"));
    currentScope->DefineSymbol(std::make_unique<BuiltInSymbol>("string"));

    symbolTables.push_back(std::move(globalScope));
}

SymbolTable* SemanticAnalyzer::CreateNewScope(const std::string& name) {
    auto newScope = std::make_unique<SymbolTable>(name, currentScope->GetScopeLevel() + 1, currentScope);
    currentScope = newScope.get();
    symbolTables.push_back(std::move(newScope));
    return currentScope;
}

void SemanticAnalyzer::ExitScope() {
    if (currentScope->GetParentScope()) {
        currentScope = currentScope->GetParentScope();
    }
}

void SemanticAnalyzer::Analyze(ASTNode& root) {
    hasError = false;
    root.Accept(*this);
}

const Symbol* SemanticAnalyzer::GetNodeType(const ASTNode* node) const {
    auto it = nodeTypes.find(node);
    return it != nodeTypes.end() ? it->second : nullptr;
}

void SemanticAnalyzer::SetNodeType(const ASTNode* node, const Symbol* type) {
    nodeTypes[node] = type;
}

void SemanticAnalyzer::Visit(ProgramNode& node) {
    for (const auto& decl : node.declarations) {
        decl->Accept(*this);
    }
}

void SemanticAnalyzer::Visit(DeclarationNode& node) {}

void SemanticAnalyzer::Visit(VarDeclarationNode& node) {
    Logger& logger = Logger::getInstance();
    const std::string& typeName = node.type.value;
    const Symbol* typeSymbol = currentScope->LookUpSymbol(typeName);

    if (!typeSymbol) {
        logger.error("Undefined type '" + typeName + "' at line " + std::to_string(node.type.line));
        hasError = true;
        return;
    }

    const std::string& varName = node.identifier.value;
    if (!currentScope->DefineSymbol(std::make_unique<VariableSymbol>(varName, std::to_string(currentScope->AllocateOffset()), typeSymbol))) {
        logger.error("Redefinition of variable '" + varName + "' at line " + std::to_string(node.identifier.line));
        hasError = true;
    }

    if (node.expression) {
        node.expression->Accept(*this);
        const Symbol* exprType = GetNodeType(node.expression.get());
        if (exprType && exprType->GetName() != typeSymbol->GetName()) {
            logger.error("Type mismatch in initialization of variable '" + varName +
                         "' at line " + std::to_string(node.identifier.line) +
                         ". Expected: " + typeSymbol->GetName() +
                         ", Found: " + exprType->GetName() + ".");
            hasError = true;
        }
    }

    SetNodeType(&node, typeSymbol);
}

void SemanticAnalyzer::Visit(FunctionDeclarationNode& node) {
    Logger& logger = Logger::getInstance();
    const std::string& returnTypeName = node.returnType.value;
    const Symbol* returnTypeSymbol = currentScope->LookUpSymbol(returnTypeName);

    if (!returnTypeSymbol) {
        logger.error("Undefined return type '" + returnTypeName + "' for function '" + node.functionName.value +
                     "' at line " + std::to_string(node.returnType.line));
        hasError = true;
        return;
    }

    if (currentScope->GetScopeLevel() == 0) {
        if (!currentScope->DefineSymbol(std::make_unique<FunctionSymbol>(node.functionName.value, returnTypeSymbol))) {
            logger.error("Redefinition of function '" + node.functionName.value + "' at line " + std::to_string(node.functionName.line));
            hasError = true;
            return;
        }
    }

    SymbolTable* functionScope = CreateNewScope(node.functionName.value);
    functionScope->SetReturnType(returnTypeSymbol);
    for (const auto& param : node.parameters) {
        param->Accept(*this);
    }

    if (node.body) {
        node.body->Accept(*this);
    }

    ExitScope();
}

void SemanticAnalyzer::Visit(ParamNode& node) {
    Logger& logger = Logger::getInstance();
    const std::string& paramName = node.identifier.value;
    const std::string& paramTypeName = node.type.value;
    const Symbol* paramTypeSymbol = currentScope->LookUpSymbol(paramTypeName);

    if (!paramTypeSymbol) {
        logger.error("Undefined type '" + paramTypeName + "' for parameter '" + paramName + "'");
        hasError = true;
        return;
    }

    if (!currentScope->DefineSymbol(std::make_unique<VariableSymbol>(paramName, std::to_string(currentScope->AllocateOffset()), paramTypeSymbol))) {
        logger.error("Redefinition of parameter '" + paramName + "'");
        hasError = true;
    }

    SetNodeType(&node, paramTypeSymbol);
}

void SemanticAnalyzer::Visit(ParamListNode& node) {
    for (const auto& param : node.parameters) {
        param->Accept(*this);
    }
}

void SemanticAnalyzer::Visit(ExpressionNode& node) {}

void SemanticAnalyzer::Visit(AssignmentNode& node) {
    Logger& logger = Logger::getInstance();

    node.left->Accept(*this);
    node.right->Accept(*this);

    const Symbol* leftType = GetNodeType(node.left.get());
    const Symbol* rightType = GetNodeType(node.right.get());

    if (leftType && rightType && !leftType->IsCompatibleWith(rightType)) {
        logger.error("Type mismatch in assignment at line " + std::to_string(node.op.line));
        hasError = true;
    }

    SetNodeType(&node, leftType);
}

void SemanticAnalyzer::Visit(BinaryOperationNode& node) {
    Logger& logger = Logger::getInstance();
    node.left->Accept(*this);
    node.right->Accept(*this);

    const Symbol* leftType = GetNodeType(node.left.get());
    const Symbol* rightType = GetNodeType(node.right.get());
    if (leftType && rightType) {
        if (leftType->GetName() == rightType->GetName()) {
            SetNodeType(&node, leftType);
        } else {
            logger.error("Type mismatch for operator '" + node.op.value +
                         "' at line " + std::to_string(node.op.line) +
                         ". Found: (" + leftType->GetName() + ", " + rightType->GetName() + ").");
            hasError = true;
        }
    }
}

void SemanticAnalyzer::Visit(UnaryOperationNode& node) {
    Logger& logger = Logger::getInstance();
    node.expr->Accept(*this);
    
    const Symbol* exprType = GetNodeType(node.expr.get());
    if (exprType) {
        SetNodeType(&node, exprType);
    } else {
        logger.error("Undefined type in unary operation at line " + std::to_string(node.op.line));
        hasError = true;
    }
}

void SemanticAnalyzer::Visit(LiteralNode& node) {
    if (node.literal.type == TokenType::LITERAL_INT) {
        SetNodeType(&node, currentScope->LookUpSymbol("integer"));
    } else if (node.literal.type == TokenType::LITERAL_STRING) {
        SetNodeType(&node, currentScope->LookUpSymbol("string"));
    } else if (node.literal.type == TokenType::LITERAL_CHAR) {
        SetNodeType(&node, currentScope->LookUpSymbol("char"));
    } 
}


void SemanticAnalyzer::Visit(IdentifierNode& node) {
    Logger& logger = Logger::getInstance();
    const Symbol* symbol = currentScope->LookUpSymbol(node.identifier.value);
    if (!symbol) {
        logger.error("Undefined identifier '" + node.identifier.value + "' at line " + std::to_string(node.identifier.line));
        hasError = true;
    } else {
        SetNodeType(&node, symbol->type);
    }
}

void SemanticAnalyzer::Visit(TypeNode& node) {}

void SemanticAnalyzer::Visit(RelationalNode& node) {
    node.left->Accept(*this);
    node.right->Accept(*this);
}

void SemanticAnalyzer::Visit(EqualityNode& node) {
    node.left->Accept(*this);
    node.right->Accept(*this);
}

void SemanticAnalyzer::Visit(LogicalOrNode& node) {
    node.left->Accept(*this);
    node.right->Accept(*this);
}

void SemanticAnalyzer::Visit(LogicalAndNode& node) {
    node.left->Accept(*this);
    node.right->Accept(*this);
}

void SemanticAnalyzer::Visit(StatementNode& node) {}

void SemanticAnalyzer::Visit(IfStatementNode& node) {
    node.condition->Accept(*this);
    CreateNewScope("IF_BLOCK");
    if (node.ifBody) {
        node.ifBody->Accept(*this);
    }
    ExitScope();

    if (node.elseBody) {
        CreateNewScope("ELSE_BLOCK");
        node.elseBody->Accept(*this);
        ExitScope();
    }
}

void SemanticAnalyzer::Visit(ForStatementNode& node) {
    CreateNewScope("FOR_BLOCK");
    if (node.init) {
        node.init->Accept(*this);
    }
    if (node.condition) {
        node.condition->Accept(*this);
    }
    if (node.increment) {
        node.increment->Accept(*this);
    }
    if (node.body) {
        node.body->Accept(*this);
    }
    ExitScope();
}

void SemanticAnalyzer::Visit(ReturnStatementNode& node) {
    Logger& logger = Logger::getInstance();

    if (node.expression) {
        node.expression->Accept(*this);
    }

    SymbolTable* functionScope = currentScope;
    while (functionScope && functionScope->GetReturnType() == nullptr) {
        functionScope = functionScope->GetParentScope();
    }

    const Symbol* returnType = functionScope ? functionScope->GetReturnType() : nullptr;
    const Symbol* exprType = node.expression ? GetNodeType(node.expression.get()) : nullptr;

    if (returnType && exprType && !returnType->IsCompatibleWith(exprType)) {
        logger.error("Return type mismatch in function '" + (functionScope ? functionScope->GetScopeName() : "unknown") +
                     "'. Expected: " + (returnType ? returnType->GetName() : "void") +
                     ", Found: " + (exprType ? exprType->GetName() : "undefined"));
        hasError = true;
    }
}

void SemanticAnalyzer::Visit(PrintStatementNode& node) {
    node.exprList->Accept(*this);
}

void SemanticAnalyzer::Visit(ExpressionStatementNode& node) {
    if (node.expression) {
        node.expression->Accept(*this);
    }
}

void SemanticAnalyzer::Visit(CompoundStatementNode& node) {
    CreateNewScope("COMPOUND_STATEMENT");
    for (const auto& statement : node.statements) {
        statement->Accept(*this);
    }
    ExitScope();
}

void SemanticAnalyzer::Visit(ExprListNode& node) {
    for (const auto& expr : node.expressions) {
        expr->Accept(*this);
    }
}

void SemanticAnalyzer::Visit(FunctionCallNode& node) {
    Logger& logger = Logger::getInstance();

    const Symbol* functionSymbol = currentScope->LookUpSymbol(node.functionName->identifier.value);
    if (!functionSymbol) {
        logger.error("Undefined identifier '" + node.functionName->identifier.value +
                     "' at line " + std::to_string(node.functionName->identifier.line));
        hasError = true;
        return;
    }

    // TODO: validar args
    if (node.arguments) {
        node.arguments->Accept(*this);
    }

    SetNodeType(&node, functionSymbol->type);
}

void SemanticAnalyzer::Visit(IndexingNode& node) {
    node.base->Accept(*this);
    node.index->Accept(*this);
}

void SemanticAnalyzer::Render() const {
    Logger& logger = Logger::getInstance();
    for (const auto& scope : symbolTables) {
        logger.info("Symbol Table: " + scope->GetScopeName() + " (Level " + std::to_string(scope->GetScopeLevel()) + ")");
        scope->Render();
    }
}
