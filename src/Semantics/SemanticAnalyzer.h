#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "Symbol.h"
#include "../AST/ASTVisitor.h"
#include "../AST/AST.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

class SemanticAnalyzer : public ASTNodeVisitor {
private:
    std::vector<std::unique_ptr<SymbolTable>> symbolTables;
    SymbolTable* currentScope;
    bool hasError;

    SymbolTable* CreateNewScope(const std::string& name);
    void ExitScope();

    std::unordered_map<const ASTNode*, const Symbol*> nodeTypes;

    const Symbol* GetNodeType(const ASTNode* node) const;
    void SetNodeType(const ASTNode* node, const Symbol* type);

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer() override = default;

    void Analyze(ASTNode& root);

    void Visit(ProgramNode& node) override;
    void Visit(DeclarationNode& node) override;
    void Visit(VarDeclarationNode& node) override;
    void Visit(FunctionDeclarationNode& node) override;

    void Visit(ParamNode& node) override;
    void Visit(ParamListNode& node) override;

    void Visit(ExpressionNode& node) override;
    void Visit(AssignmentNode& node) override;
    void Visit(BinaryOperationNode& node) override;
    void Visit(UnaryOperationNode& node) override;
    void Visit(LiteralNode& node) override;
    void Visit(IdentifierNode& node) override;
    void Visit(TypeNode& node) override;

    void Visit(RelationalNode& node) override;
    void Visit(EqualityNode& node) override;
    void Visit(LogicalOrNode& node) override;
    void Visit(LogicalAndNode& node) override;

    void Visit(StatementNode& node) override;
    void Visit(IfStatementNode& node) override;
    void Visit(ForStatementNode& node) override;
    void Visit(ReturnStatementNode& node) override;
    void Visit(PrintStatementNode& node) override;
    void Visit(ExpressionStatementNode& node) override;
    void Visit(CompoundStatementNode& node) override;

    void Visit(ExprListNode& node) override;

    void Visit(FunctionCallNode& node) override;
    void Visit(IndexingNode& node) override;

    void Render() const;
    bool HasError() const { return hasError; }
};

#endif
