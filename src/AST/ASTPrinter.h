#include "ASTVisitor.h"
#include "AST.h"
#include "../scanner/token.h"

class ASTPrinter : public ASTNodeVisitor {
private:
    int indentation = 0;

    void Indent() { indentation += 2; }
    void Dedent() { indentation -= 2; }
    void PrintIndent() const { std::cout << std::string(indentation, ' '); }

public:
    void Visit(ProgramNode& node) override {
        PrintIndent();
        std::cout << "ProgramNode\n";
        Indent();
        for (const auto& decl : node.declarations) {
            decl->Accept(*this);
        }
        Dedent();
    }

    void Visit(DeclarationNode& node) override {
        PrintIndent();
        std::cout << "DeclarationNode\n";
    }

    void Visit(VarDeclarationNode& node) override {
        PrintIndent();
        std::cout << "VarDeclarationNode: " << node.identifier.value << " of type " << node.type.value << "\n";
        if (node.expression) {
            Indent();
            std::cout << "Assigned Expression:\n";
            node.expression->Accept(*this);
            Dedent();
        }
    }

    void Visit(FunctionDeclarationNode& node) override {
        PrintIndent();
        std::cout << "FunctionDeclarationNode: " << node.functionName.value << " returns " << node.returnType.value << "\n";
        Indent();
        std::cout << "Parameters:\n";
        for (const auto& param : node.parameters) {
            param->Accept(*this);
        }
        std::cout << "Function Body:\n";
        if (node.body) node.body->Accept(*this);
        Dedent();
    }

    // Parámetros
    void Visit(ParamNode& node) override {
        PrintIndent();
        std::cout << "ParamNode: " << node.identifier.value << " of type " << node.type.value << "\n";
    }

    void Visit(ParamListNode& node) override {
        PrintIndent();
        std::cout << "ParamListNode\n";
        Indent();
        for (const auto& param : node.parameters) {
            param->Accept(*this);
        }
        Dedent();
    }

    // Expresiones y operaciones
    void Visit(ExpressionNode& node) override {
        PrintIndent();
        std::cout << "ExpressionNode\n";
    }

    void Visit(AssignmentNode& node) override {
        PrintIndent();
        std::cout << "AssignmentNode: " << node.op.value << "\n";
        Indent();
        std::cout << "Left Side:\n";
        node.left->Accept(*this);
        std::cout << "Right Side:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(BinaryOperationNode& node) override {
        PrintIndent();
        std::cout << "BinaryOperationNode: " << node.op.value << "\n";
        Indent();
        std::cout << "Left Operand:\n";
        node.left->Accept(*this);
        std::cout << "Right Operand:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(UnaryOperationNode& node) override {
        PrintIndent();
        std::cout << "UnaryOperationNode: " << node.op.value << "\n";
        Indent();
        node.expr->Accept(*this);
        Dedent();
    }

    void Visit(LiteralNode& node) override {
        PrintIndent();
        std::cout << "LiteralNode: " << node.literal.value << " (" << tokenTypeMap.at(node.literal.type) << ")\n";
    }

    void Visit(IdentifierNode& node) override {
        PrintIndent();
        std::cout << "IdentifierNode: " << node.identifier.value << " (" << tokenTypeMap.at(node.identifier.type) << ")\n";
    }

    void Visit(TypeNode& node) override {
        PrintIndent();
        std::cout << "TypeNode: " << node.type.value << (node.isArray ? "[]" : "") << "\n";
    }

    void Visit(RelationalNode& node) override {
        PrintIndent();
        std::cout << "RelationalNode: " << node.op.value << "\n";
        Indent();
        std::cout << "Left Operand:\n";
        node.left->Accept(*this);
        std::cout << "Right Operand:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(EqualityNode& node) override {
        PrintIndent();
        std::cout << "EqualityNode: " << node.op.value << "\n";
        Indent();
        std::cout << "Left Operand:\n";
        node.left->Accept(*this);
        std::cout << "Right Operand:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(LogicalOrNode& node) override {
        PrintIndent();
        std::cout << "LogicalOrNode: " << node.op.value << " (||)\n";
        Indent();
        std::cout << "Left Operand:\n";
        node.left->Accept(*this);
        std::cout << "Right Operand:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(LogicalAndNode& node) override {
        PrintIndent();
        std::cout << "LogicalAndNode: " << node.op.value << " (&&)\n";
        Indent();
        std::cout << "Left Operand:\n";
        node.left->Accept(*this);
        std::cout << "Right Operand:\n";
        node.right->Accept(*this);
        Dedent();
    }

    void Visit(StatementNode& node) override {
        PrintIndent();
        std::cout << "StatementNode\n";
    }

    void Visit(IfStatementNode& node) override {
        PrintIndent();
        std::cout << "IfStatementNode\n";
        Indent();
        std::cout << "Condition:\n";
        node.condition->Accept(*this);
        std::cout << "If Body:\n";
        node.ifBody->Accept(*this);
        if (node.elseBody) {
            std::cout << "Else Body:\n";
            node.elseBody->Accept(*this);
        }
        Dedent();
    }

    void Visit(ForStatementNode& node) override {
        PrintIndent();
        std::cout << "ForStatementNode\n";
        Indent();
        std::cout << "Initialization:\n";
        node.init->Accept(*this);
        std::cout << "Condition:\n";
        node.condition->Accept(*this);
        std::cout << "Increment:\n";
        node.increment->Accept(*this);
        std::cout << "Body:\n";
        node.body->Accept(*this);
        Dedent();
    }

    void Visit(ReturnStatementNode& node) override {
        PrintIndent();
        std::cout << "ReturnStatementNode\n";
        Indent();
        node.expression->Accept(*this);
        Dedent();
    }

    void Visit(PrintStatementNode& node) override {
        PrintIndent();
        std::cout << "PrintStatementNode\n";
        Indent();
        node.exprList->Accept(*this);
        Dedent();
    }

    void Visit(ExpressionStatementNode& node) override {
        PrintIndent();
        std::cout << "ExpressionStatementNode\n";
        Indent();
        node.expression->Accept(*this);
        Dedent();
    }

    void Visit(CompoundStatementNode& node) override {
        PrintIndent();
        std::cout << "CompoundStatementNode\n";
        Indent();
        for (const auto& stmt : node.statements) {
            stmt->Accept(*this);
        }
        Dedent();
    }

    void Visit(ExprListNode& node) override {
        PrintIndent();
        std::cout << "ExprListNode\n";
        Indent();
        for (const auto& expr : node.expressions) {
            expr->Accept(*this);
        }
        Dedent();
    }
};
