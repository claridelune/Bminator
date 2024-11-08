#ifndef  AST_H
#define  AST_H

/*
-> : hereda
clases
- declaraciones y funciones
    - ProgramNode (root)
    - DeclarationNode (base cualquier declaracion?)
    - VarDeclarationNode -> DeclarationNode
    - FunctionDeclarationNode -> DeclarationNode
- expresiones y operaciones
    - ExpressionNode (clase abstracta)
    - AssigmentNode (expresion = expresion) -> ExpressionNode
    - BinaryOperationNode (+, -, *, /, etc) -> ExpressionNode
    - UnaryOperationNode (-, !) -> ExpressionNode
    - LiteralNode -> ExpressionNode
    - IdentifierNode -> ExpressionNode
- tipos
    - TypeNode (integer, ...)
- ralciones y logicas
    - RelationalNode (<, >, ...) -> BinaryOperationNode
    - EqualityNode (==, !=) -> BinaryOperationNode
    - LogicalAndNode  -> BinaryOperationNode
    - LogicalOrNode -> BinaryOperationNode
- statements
    - StatementNode (base)
    - IfStatementNode (if, else)
    - ForStatementNode
    - ReturnStatementNode
    - PrintStatementNode
    - ExpressionStatementNode
    - CompoundStatementNode
- parametros, listas de expresiones
    - ParamNode
    - ParamListNode
    - ExprListNode
* */

#include <memory>
#include <vector>
#include "ASTVisitor.h"
#include "../scanner/token.h"

template<typename T>
using UnqPtr = std::unique_ptr<T>;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    std::string parentID;
    virtual void Accept(ASTNodeVisitor& v) = 0;
    virtual void SetChildrenPrintID(const std::string& pID) = 0;
};

class ProgramNode : public ASTNode {
public:
    std::vector<UnqPtr<ASTNode>> declarations;

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        for (auto& decl : declarations) decl->parentID = pID;
    }
    void AddDeclaration(UnqPtr<ASTNode> declaration) {
        declarations.push_back(std::move(declaration));
    }
};

class DeclarationNode : public ASTNode {};

class VarDeclarationNode : public DeclarationNode {
public:
    Token type;
    Token identifier;
    UnqPtr<ASTNode> expression;  // opcional

    VarDeclarationNode(const Token& t, const Token& id, UnqPtr<ASTNode> expr = nullptr)
        : type(t), identifier(id), expression(std::move(expr)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        if (expression) expression->parentID = pID;
    }
};

class ParamNode : public ASTNode {
public:
    Token type;
    Token identifier;

    ParamNode(const Token& t, const Token& id) : type(t), identifier(id) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) noexcept override {}
};

class FunctionDeclarationNode : public DeclarationNode {
public:
    Token returnType;
    Token functionName;
    std::vector<UnqPtr<ParamNode>> parameters;
    UnqPtr<ASTNode> body;

    FunctionDeclarationNode(const Token& retType, const Token& name, std::vector<UnqPtr<ParamNode>> params, UnqPtr<ASTNode> b)
        : returnType(retType), functionName(name), parameters(std::move(params)), body(std::move(b)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        for (auto& param : parameters) param->parentID = pID;
        if (body) body->parentID = pID;
    }
};

class ExpressionNode : public ASTNode {};

class AssignmentNode : public ExpressionNode {
public:
    UnqPtr<ASTNode> left;
    Token op;
    UnqPtr<ASTNode> right;

    AssignmentNode(UnqPtr<ASTNode> l, const Token& o, UnqPtr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        left->parentID = pID;
        right->parentID = pID;
    }
};

class BinaryOperationNode : public ExpressionNode {
public:
    UnqPtr<ASTNode> left;
    Token op;
    UnqPtr<ASTNode> right;

    BinaryOperationNode(UnqPtr<ASTNode> l, const Token& o, UnqPtr<ASTNode> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        left->parentID = pID;
        right->parentID = pID;
    }
};

class UnaryOperationNode : public ExpressionNode {
public:
    Token op;
    UnqPtr<ASTNode> expr;

    UnaryOperationNode(const Token& o, UnqPtr<ASTNode> e) : op(o), expr(std::move(e)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        expr->parentID = pID;
    }
};

class LiteralNode : public ExpressionNode {
public:
    Token literal;

    LiteralNode(const Token& lit) : literal(lit) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) noexcept override {}
};

class IdentifierNode : public ExpressionNode {
public:
    Token identifier;

    IdentifierNode(const Token& id) : identifier(id) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) noexcept override {}
};

class TypeNode : public ASTNode {
public:
    Token type;
    bool isArray;

    TypeNode(const Token& t, bool array = false) : type(t), isArray(array) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) noexcept override {}
};

class StatementNode : public ASTNode {};

class IfStatementNode : public StatementNode {
public:
    UnqPtr<ASTNode> condition;
    UnqPtr<ASTNode> ifBody;
    UnqPtr<ASTNode> elseBody;

    IfStatementNode(UnqPtr<ASTNode> cond, UnqPtr<ASTNode> ifB, UnqPtr<ASTNode> elseB = nullptr)
        : condition(std::move(cond)), ifBody(std::move(ifB)), elseBody(std::move(elseB)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        condition->parentID = pID;
        ifBody->parentID = pID;
        if (elseBody) elseBody->parentID = pID;
    }
};

class ForStatementNode : public StatementNode {
public:
    UnqPtr<ASTNode> init;
    UnqPtr<ASTNode> condition;
    UnqPtr<ASTNode> increment;
    UnqPtr<ASTNode> body;

    ForStatementNode(UnqPtr<ASTNode> i, UnqPtr<ASTNode> cond, UnqPtr<ASTNode> inc, UnqPtr<ASTNode> b)
        : init(std::move(i)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(b)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        init->parentID = pID;
        condition->parentID = pID;
        increment->parentID = pID;
        body->parentID = pID;
    }
};

class ReturnStatementNode : public StatementNode {
public:
    UnqPtr<ASTNode> expression;

    ReturnStatementNode(UnqPtr<ASTNode> expr) : expression(std::move(expr)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        expression->parentID = pID;
    }
};

class PrintStatementNode : public StatementNode {
public:
    UnqPtr<ASTNode> exprList;

    PrintStatementNode(UnqPtr<ASTNode> el) : exprList(std::move(el)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        exprList->parentID = pID;
    }
};

class ExprListNode : public ASTNode {
public:
    std::vector<UnqPtr<ASTNode>> expressions;

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        for (auto& expr : expressions) expr->parentID = pID;
    }
    void AddExpression(UnqPtr<ASTNode> expr) {
        expressions.push_back(std::move(expr));
    }
};

class CompoundStatementNode : public StatementNode {
public:
    std::vector<UnqPtr<ASTNode>> statements;

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        for (auto& stmt : statements) stmt->parentID = pID;
    }
    void AddStatement(UnqPtr<ASTNode> statement) {
        statements.push_back(std::move(statement));
    }
};

class RelationalNode : public BinaryOperationNode {
public:
    RelationalNode(UnqPtr<ASTNode> left, const Token& op, UnqPtr<ASTNode> right)
        : BinaryOperationNode(std::move(left), op, std::move(right)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
};

class EqualityNode : public BinaryOperationNode {
public:
    EqualityNode(UnqPtr<ASTNode> left, const Token& op, UnqPtr<ASTNode> right)
        : BinaryOperationNode(std::move(left), op, std::move(right)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
};

class LogicalOrNode : public BinaryOperationNode {
public:
    LogicalOrNode(UnqPtr<ASTNode> l, const Token& o, UnqPtr<ASTNode> r)
        : BinaryOperationNode(std::move(l), o, std::move(r)) {}

    void Accept(ASTNodeVisitor& visitor) override { visitor.Visit(*this); }
};

class LogicalAndNode : public BinaryOperationNode {
public:
    LogicalAndNode(UnqPtr<ASTNode> l, const Token& o, UnqPtr<ASTNode> r)
        : BinaryOperationNode(std::move(l), o, std::move(r)) {}

    void Accept(ASTNodeVisitor& visitor) override { visitor.Visit(*this); }
};

class ExpressionStatementNode : public StatementNode {
public:
    UnqPtr<ASTNode> expression;  // opcional 

    ExpressionStatementNode(UnqPtr<ASTNode> expr = nullptr) 
        : expression(std::move(expr)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        if (expression) expression->parentID = pID;
    }
};

class ParamListNode : public ASTNode {
public:
    std::vector<UnqPtr<ParamNode>> parameters;

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        for (auto& param : parameters) param->parentID = pID;
    }
    void AddParameter(UnqPtr<ParamNode> param) {
        parameters.push_back(std::move(param));
    }
};

class FunctionCallNode : public ASTNode {
    public:
        UnqPtr<IdentifierNode> functionName;
        UnqPtr<ExprListNode> arguments;

        FunctionCallNode(UnqPtr<IdentifierNode> fnName, UnqPtr<ExprListNode> args)
            : functionName(std::move(fnName)), arguments(std::move(args)) {}
        
        void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
        void SetChildrenPrintID(const std::string& pID) override {
            functionName->parentID = pID;
            if (arguments) {
                arguments->parentID = pID;
            }
        }
};

class IndexingNode : public ASTNode {
public:
    UnqPtr<ASTNode> base;
    UnqPtr<ASTNode> index;

    IndexingNode(UnqPtr<ASTNode> b, UnqPtr<ASTNode> i)
        : base(std::move(b)), index(std::move(i)) {}

    void Accept(ASTNodeVisitor& v) override { v.Visit(*this); }
    void SetChildrenPrintID(const std::string& pID) override {
        base->parentID = pID;
        index->parentID = pID;
    }
};


#endif


