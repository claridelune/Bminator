#include "ASTPrinterJson.h"
#include "../scanner/token.h"
#include "../utils/Utility.h"
#include <cassert>

ASTPrinterJson::ASTPrinterJson(const std::string& filename) : nodeId(0) {
    out.open(filename);
    if (out) {
        out << "[\n";
    }
}

ASTPrinterJson::~ASTPrinterJson() {
    if (out) {
        GenerateJSONFooter();
        out << "\n]\n";
        out.close();
    }
}

std::string ASTPrinterJson::EscapeString(const std::string& str) const {
    std::string escaped = str;
    size_t pos = 0;
    while ((pos = escaped.find("\"", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\"");
        pos += 2;
    }
    return "\"" + escaped + "\"";
}

void ASTPrinterJson::WriteNode(const std::string& type, const std::string& content, const ASTNode* node, const std::string& parentID) {
    const std::string nodeID = Util::GenerateID(node, type.c_str());
    out << "{ \"id\": \"" << nodeID << "\", \"parent\": \"" << parentID << "\", \"type\": " 
        << EscapeString(type) << ", \"content\": " << EscapeString(content) << " },\n";
    config.push_back(nodeID);
}

std::string ASTPrinterJson::GenerateJSONHeader(const ASTNode* root, const char* rootID) {
    const std::string id = Util::GenerateID(root, rootID);
    out << "{ \"id\": \"" << id << "\", \"type\": \"ROOT\", \"content\": \"ROOT\" },\n";
    config.push_back(id);
    return id;
}

void ASTPrinterJson::GenerateJSONFooter() {
    out << "{ \"simple_chart_config\": [";
    for (size_t i = 0; i < config.size(); ++i) {
        if (i > 0) out << ", ";
        out << "\"" << config[i] << "\"";
    }
    out << "] }";
}

void ASTPrinterJson::PrintAST(ProgramNode* root) {
    if (!root) return;
    std::string rootID = GenerateJSONHeader(root, "ROOT");
    root->SetChildrenPrintID(rootID);
    root->Accept(*this);
}

void ASTPrinterJson::Visit(ProgramNode& node) {
    const std::string programNodeID = Util::GenerateID(&node, "ProgramNode");
    WriteNode("ProgramNode", "Program Start", &node, "");
    
    for (const auto& decl : node.declarations) {
        decl->parentID = programNodeID;
        decl->Accept(*this);
    }
}

void ASTPrinterJson::Visit(DeclarationNode& node) {
    WriteNode("DeclarationNode", "Declaration", &node, node.parentID);
}

void ASTPrinterJson::Visit(VarDeclarationNode& node) {
    const std::string varDeclID = Util::GenerateID(&node, "VarDeclarationNode");
    WriteNode("VarDeclarationNode", node.identifier.value + "(" + node.type.value + ")", &node, node.parentID);

    if (node.expression) {
        node.expression->parentID = varDeclID;
        node.expression->Accept(*this);
    }
}

void ASTPrinterJson::Visit(FunctionDeclarationNode& node) {
    const std::string functionNodeID = Util::GenerateID(&node, "FunctionDeclarationNode");
    WriteNode("FunctionDeclarationNode", node.functionName.value + "(" + node.returnType.value + ")", &node, node.parentID);

    for (const auto& param : node.parameters) {
        param->parentID = functionNodeID;
        param->Accept(*this);
    }

    if (node.body) {
        node.body->parentID = functionNodeID;
        node.body->Accept(*this);
    }
}

void ASTPrinterJson::Visit(ParamNode& node) {
    WriteNode("ParamNode", node.identifier.value + "(" + node.type.value + ")", &node, node.parentID);
}

void ASTPrinterJson::Visit(ParamListNode& node) {
    WriteNode("ParamListNode", "Parameter List", &node, node.parentID);
    for (const auto& param : node.parameters) {
        param->parentID = node.parentID;
        param->Accept(*this);
    }
}

void ASTPrinterJson::Visit(ExpressionNode& node) {
    WriteNode("ExpressionNode", "Expression", &node, node.parentID);
}

void ASTPrinterJson::Visit(AssignmentNode& node) {
    const std::string assignID = Util::GenerateID(&node, "AssignmentNode");
    WriteNode("AssignmentNode", "Assignment", &node, node.parentID);
    node.left->parentID = assignID;
    node.left->Accept(*this);
    node.right->parentID = assignID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(BinaryOperationNode& node) {
    const std::string binOpID = Util::GenerateID(&node, "BinaryOperationNode");
    WriteNode("BinaryOperationNode", node.op.value, &node, node.parentID);
    node.left->parentID = binOpID;
    node.left->Accept(*this);
    node.right->parentID = binOpID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(UnaryOperationNode& node) {
    const std::string unaryOpID = Util::GenerateID(&node, "UnaryOperationNode");
    WriteNode("UnaryOperationNode", node.op.value, &node, node.parentID);
    node.expr->parentID = unaryOpID;
    node.expr->Accept(*this);
}

void ASTPrinterJson::Visit(LiteralNode& node) {
    WriteNode("LiteralNode", node.literal.value, &node, node.parentID);
}

void ASTPrinterJson::Visit(IdentifierNode& node) {
    WriteNode("IdentifierNode", node.identifier.value, &node, node.parentID);
}

void ASTPrinterJson::Visit(TypeNode& node) {
    WriteNode("TypeNode", node.type.value, &node, node.parentID);
}

void ASTPrinterJson::Visit(RelationalNode& node) {
    const std::string relOpID = Util::GenerateID(&node, "RelationalNode");
    WriteNode("RelationalNode", node.op.value, &node, node.parentID);
    node.left->parentID = relOpID;
    node.left->Accept(*this);
    node.right->parentID = relOpID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(EqualityNode& node) {
    const std::string eqOpID = Util::GenerateID(&node, "EqualityNode");
    WriteNode("EqualityNode", node.op.value, &node, node.parentID);
    node.left->parentID = eqOpID;
    node.left->Accept(*this);
    node.right->parentID = eqOpID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(LogicalOrNode& node) {
    const std::string orNodeID = Util::GenerateID(&node, "LogicalOrNode");
    WriteNode("LogicalOrNode", "||", &node, node.parentID);
    node.left->parentID = orNodeID;
    node.left->Accept(*this);
    node.right->parentID = orNodeID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(LogicalAndNode& node) {
    const std::string andNodeID = Util::GenerateID(&node, "LogicalAndNode");
    WriteNode("LogicalAndNode", "&&", &node, node.parentID);
    node.left->parentID = andNodeID;
    node.left->Accept(*this);
    node.right->parentID = andNodeID;
    node.right->Accept(*this);
}

void ASTPrinterJson::Visit(StatementNode& node) {
    WriteNode("StatementNode", "Statement", &node, node.parentID);
}

void ASTPrinterJson::Visit(IfStatementNode& node) {
    const std::string ifNodeID = Util::GenerateID(&node, "IfStatementNode");
    WriteNode("IfStatementNode", "If Statement", &node, node.parentID);
    node.condition->parentID = ifNodeID;
    node.condition->Accept(*this);
    node.ifBody->parentID = ifNodeID;
    node.ifBody->Accept(*this);
    if (node.elseBody) {
        node.elseBody->parentID = ifNodeID;
        node.elseBody->Accept(*this);
    }
}

void ASTPrinterJson::Visit(ForStatementNode& node) {
    const std::string forNodeID = Util::GenerateID(&node, "ForStatementNode");
    WriteNode("ForStatementNode", "For Loop", &node, node.parentID);
    node.init->parentID = forNodeID;
    node.init->Accept(*this);
    node.condition->parentID = forNodeID;
    node.condition->Accept(*this);
    node.increment->parentID = forNodeID;
    node.increment->Accept(*this);
    node.body->parentID = forNodeID;
    node.body->Accept(*this);
}

void ASTPrinterJson::Visit(ReturnStatementNode& node) {
    const std::string returnID = Util::GenerateID(&node, "ReturnStatementNode");
    WriteNode("ReturnStatementNode", "Return", &node, node.parentID);
    if (node.expression) {
        node.expression->parentID = returnID;
        node.expression->Accept(*this);
    }
}

void ASTPrinterJson::Visit(PrintStatementNode& node) {
    const std::string printID = Util::GenerateID(&node, "PrintStatementNode");
    WriteNode("PrintStatementNode", "Print Statement", &node, node.parentID);
    node.exprList->parentID = printID;
    node.exprList->Accept(*this);
}

void ASTPrinterJson::Visit(ExpressionStatementNode& node) {
    // WriteNode("ExpressionStatementNode", "Expression Statement", &node, node.parentID);
    node.expression->parentID = node.parentID;
    node.expression->Accept(*this);
}

void ASTPrinterJson::Visit(CompoundStatementNode& node) {
    // WriteNode("CompoundStatementNode", "Compound Statement", &node, node.parentID);
    for (const auto& stmt : node.statements) {
        stmt->parentID = node.parentID;
        stmt->Accept(*this);
    }
}

void ASTPrinterJson::Visit(ExprListNode& node) {
    // WriteNode("ExprListNode", "Expression List", &node, node.parentID);
    for (const auto& expr : node.expressions) {
        expr->parentID = node.parentID;
        expr->Accept(*this);
    }
}

void ASTPrinterJson::Visit(FunctionCallNode& node) {
    const std::string funcCallID = Util::GenerateID(&node, "FunctionCallNode");
    WriteNode("FunctionCallNode", node.functionName->identifier.value, &node, node.parentID);
    if (node.arguments) {
        for (const auto& arg : node.arguments->expressions) {
            arg->parentID = funcCallID;
            arg->Accept(*this);
        }
    }
}

void ASTPrinterJson::Visit(IndexingNode& node) {
    const std::string indexNodeID = Util::GenerateID(&node, "IndexingNode");
    WriteNode("IndexingNode", "Indexing", &node, node.parentID);
    node.base->parentID = indexNodeID;
    node.base->Accept(*this);
    node.index->parentID = indexNodeID;
    node.index->Accept(*this);
}
