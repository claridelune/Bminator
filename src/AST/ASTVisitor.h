#ifndef AST_VISITOR_H
#define AST_VISITOR_H

class ProgramNode;
class DeclarationNode;
class VarDeclarationNode;
class FunctionDeclarationNode;
class ParamNode;
class ParamListNode;
class AssignmentNode;
class ExpressionNode;
class BinaryOperationNode;
class UnaryOperationNode;
class LiteralNode;
class IdentifierNode;
class TypeNode;
class RelationalNode;
class EqualityNode;
class LogicalOrNode;
class LogicalAndNode;
class StatementNode;
class IfStatementNode;
class ForStatementNode;
class ReturnStatementNode;
class PrintStatementNode;
class ExpressionStatementNode;
class CompoundStatementNode;
class ExprListNode;

class ASTNodeVisitor {
public:
    virtual ~ASTNodeVisitor() = default;

    virtual void Visit(ProgramNode& node) = 0;
    virtual void Visit(DeclarationNode& node) = 0;
    virtual void Visit(VarDeclarationNode& node) = 0;
    virtual void Visit(FunctionDeclarationNode& node) = 0;

    virtual void Visit(ParamNode& node) = 0;
    virtual void Visit(ParamListNode& node) = 0;

    virtual void Visit(ExpressionNode& node) = 0;
    virtual void Visit(AssignmentNode& node) = 0;
    virtual void Visit(BinaryOperationNode& node) = 0;
    virtual void Visit(UnaryOperationNode& node) = 0;
    virtual void Visit(LiteralNode& node) = 0;
    virtual void Visit(IdentifierNode& node) = 0;
    virtual void Visit(TypeNode& node) = 0;

    virtual void Visit(RelationalNode& node) = 0;
    virtual void Visit(EqualityNode& node) = 0;
    virtual void Visit(LogicalOrNode& node) = 0;
    virtual void Visit(LogicalAndNode& node) = 0;

    virtual void Visit(StatementNode& node) = 0;
    virtual void Visit(IfStatementNode& node) = 0;
    virtual void Visit(ForStatementNode& node) = 0;
    virtual void Visit(ReturnStatementNode& node) = 0;
    virtual void Visit(PrintStatementNode& node) = 0;
    virtual void Visit(ExpressionStatementNode& node) = 0;
    virtual void Visit(CompoundStatementNode& node) = 0;

    virtual void Visit(ExprListNode& node) = 0;
};

#endif
