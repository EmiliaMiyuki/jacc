#ifndef _MIYUKI_ASTBUILDER_H
#define _MIYUKI_ASTBUILDER_H

#include "parse/ifparser.h"
#include "astsym.h"

// this file defines AST Builder, which builds Anstract Syntax Tree
// this class is common use in both preprocessor and parser

namespace Miyuki::AST {

    using namespace Miyuki::Parse;

    class ASTBuilder : public IParser {
    public:
        virtual TokenPtr buildXXX() { assert( false && "do not use this function, just a sample function." ); }

        // expression  (ref A.2.1)
        CommaExpressionPtr       expression();
        ConditionalExpressionPtr constantExpression();
        ConditionalExpressionPtr conditionalExpression();
        LogicalORExpressionPtr   logicalORExpression();
        LogicalANDExpressionPtr  logicalANDExpression();
        ArithmeticPtr            inclusiveORExpression();
        ArithmeticPtr            exclusiveORExpression();
        ArithmeticPtr            andExpression();
        ArithmeticPtr            equalityExpression();
        ArithmeticPtr            relationalExpression();
        ArithmeticPtr            shiftExpression();
        ArithmeticPtr            additiveExpression();
        ArithmeticPtr            multiplicativeExpression();
        CastExpressionPtr        castExpression();
        UnaryPtr                 unaryExpression();
        PostfixExpressionPtr     postfixExpression();
        PrimaryExpressionPtr     primaryExpression();
        AssignmentExpressionPtr  assignmentExpression();
        ArgumentExpressionListPtr argumentExpressionList();

        // statements  (ref A.2.2)
        DeclarationPtr           declaration();
        DeclarationSpecifierPtr  declarationSpecifiers();
        InitDeclaratorListPtr    initDeclaratorList();
        InitDeclaratorPtr        initDeclarator();
        TypeSpecifierPtr         typeSpecifier();
        StructOrUnionSpecifierPtr structOrUnionSpecifier();
        EnumSpecifierPtr         enumSpecifier();
        StructDeclarationListPtr structDeclarationList();
        StructDeclarationPtr     structDeclaration();
        SpecifierAndQualifierListPtr specifierQualifierList();
        StructDeclaratorListPtr  structDeclaratorList();
        StructDeclaratorPtr      structDeclarator();
        EnumeratorListPtr        enumeratorList();
        EnumeratorPtr            enumerator();
        DeclaratorPtr            declarator();
        DirectDeclaratorPtr      directDeclarator();
        PointerDeclPtr           pointerDecl();
        TypeQualifierListPtr     typeQualifierList();
        ParameterTypeListPtr     parameterTypeList();
        ParameterListPtr         parameterList();
        ParameterDecleartionPtr  parameterDecleartion();
        WordTokenListPtr         identifierList();
        TypeNamePtr              typeName();
        AbstractDeclaratorPtr    abstractDeclarator();
        DirectAbstractDeclaratorPtr directAbstractDeclarator();
        InitializerPtr           initializer();
        InitializerListPtr       initializerList();
        DesignationPtr           designation();
        DesignatorListPtr        designatorList();
        DesignatorPtr            designator();
    };

}

#endif