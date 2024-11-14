#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <sstream>

#include "../AST/ASTPrinterJson.h"

namespace Util
{
    inline std::string GenerateID(const ASTNode* node, const char* ID)
    {
        std::stringstream ss;
        ss << static_cast<const void*>(node);
        std::string id = ss.str();
        id.insert(0, ID);
        return id;
    }
}

#endif
