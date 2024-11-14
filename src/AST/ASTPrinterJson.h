#ifndef AST_PRINTER_JSON_H
#define AST_PRINTER_JSON_H

#include <vector>
#include <fstream>
#include <cassert>
#include "ASTVisitor.h"
#include "AST.h"
#include "../scanner/token.h"

class ASTPrinterJson : public ASTNodeVisitor {
private:
    std::vector<std::string> config;
    std::ofstream out;
    int nodeId;

    std::string EscapeString(const std::string& str) const;
    void WriteNode(const std::string& type, const std::string& content, const ASTNode* node, const std::string& parentID);
    std::string GenerateJSONHeader(const ASTNode* root, const char* rootID);
    void GenerateJSONFooter();

public:
    ASTPrinterJson(const std::string& filename);
    ~ASTPrinterJson();

    void PrintAST(ProgramNode* root);

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
};

#endif
