#ifndef INERTIA_ASTNODE_HPP
#define INERTIA_ASTNODE_HPP

#include <cstdint>

namespace Inertia{
    class ASTNode{
    public:
        enum ASTKind {LITERAL, OPERATOR} kind;
        ASTNode* left;
        ASTNode* right;
    };

    class ASTOperator : public ASTNode{
    public:
        
    };

    class ASTLiteral : public ASTNode{
    public:
        intmax_t value;    
    };
}

#endif // INERTIA_ASTNODE_HPP
