#ifndef _MIYUKI_FIRST_FOLLOW_DEF_H
#define _MIYUKI_FIRST_FOLLOW_DEF_H

#define ISTAG(x)      look->is(Tag:: x)
#define LIS(x)        look->is(x)

////////////////////////   expression   //////////////////////////////
// FIRST(primary-expression)={ identifier , constant , string-literal , ( ,  }
#define FIRST_PRIMARY_EXPRESSION() \
    ( ISTAG(Identifier) || ISTAG(Constant) || ISTAG(Character) || ISTAG(StringLiteral) || LIS('(') )

// FIRST(expression)={ identifier , constant , string-literal , ( , ++ , -- , & , * , + , - , ~ , ! , sizeof ,  }
#define FIRST_EXPRESSION() \
    ( FIRST_PRIMARY_EXPRESSION() || ISTAG(Increase) || ISTAG(Decrease) || LIS('&') || LIS('*') ||\
        LIS('+') || LIS('-') || LIS('~') || LIS('!') || ISTAG(Sizeof) )

// FIRST(unary-operator)={ & , * , + , - , ~ , ! ,  }
#define FIRST_UNARY_OPERATOR() \
    ( LIS('&') || LIS('*') || LIS('+') || LIS('-') || LIS('~') || LIS('!') )

// FIRST(assignment-operator)={ ONE OF = *= /= %= += -= <<= >>= &= ^= |= }
#define F_ASSIGNMENT_OPERATOR() \
    ( LIS('=') || ISTAG(MulpileAssign) || ISTAG(DivideAssign) || ISTAG(AddAssign) || ISTAG(SubAssign) || \
    ISTAG(ModAssign) || ISTAG(LeftShiftAssign) || ISTAG(RightShiftAssign) || ISTAG(BitwiseAndAssign) || \
    ISTAG(XorAssign) || ISTAG(BitwiseOrAssign) )

// FOLLOW(constant-expression)={$ ,  }
#define FOLLOW_CONSTANT_EXPRESSION() \
    ( ISTAG(EndOfFile) )

// FOLLOW(expression)=                            {, , : , ] , ) ,  }
#define FOLLOW_EXPRESSION() ( LIS(','), LIS(':'), LIS(']'), LIS(')') )

// FOLLOW(conditional-expression)=            {$ , , , : , ] , ) ,  }
#define FOLLOW_CONDITIONAL_EXPRESSION() \
    ( FOLLOW_CONSTANT_EXPRESSION() || FOLLOW_EXPRESSION() )

// FOLLOW(logical-OR-expression)=             {$ , , , : , ] , ) , ? , || ,  }
#define FOLLOW_LOGICAL_OR_EXPRESSION() \
    ( FOLLOW_CONSTANT_EXPRESSION() ||  FOLLOW_EXPRESSION() || LIS('?') || ISTAG(Or) )

// FOLLOW(inclusive-OR-expression)=           {$ , , , : , ] , ) , ? , || , && , | ,  }
#define FOLLOW_INCLUSIVE_OR_EXPRESSION() \
    ( FOLLOW_LOGICAL_OR_EXPRESSION() || ISTAG(And) || LIS('|') )

// FOLLOW(exclusive-OR-expression)=           {$ , , , : , ] , ) , ? , || , && , | , ^ ,  }
#define FOLLOW_EXCLUSIVE_OR_EXPRESSION() \
    ( FOLLOW_INCLUSIVE_OR_EXPRESSION() || LIS('^') )

// FOLLOW(equality-expression)=               {$ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != ,  }
#define FOLLOW_EQUALITY_EXPRESSION() \
    ( FOLLOW_EXCLUSIVE_OR_EXPRESSION() || LIS('&') || ISTAG(Equal) || ISTAG(NotEqual)  )

// FOLLOW(relational-expression)=             {$ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= ,  }
#define FOLLOW_RELATIONAL_EXPRESSION() \
    ( FOLLOW_EQUALITY_EXPRESSION() || LIS('<') || LIS('>') || ISTAG(LessThanEqual) || ISTAG(GreaterThanEqual) )

// FOLLOW(shift-expression)=                  {$ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= , << , >> ,  }
#define FOLLOW_SHIFT_EXPRESSION() \
    ( FOLLOW_RELATIONAL_EXPRESSION() ||  ISTAG(LeftShift) || ISTAG(RightShift) )

// FOLLOW(additive-expression)=               {$ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= , << , >> , + , - ,  }
#define FOLLOW_ADDITIVE_EXPRESSION() \
    ( FOLLOW_SHIFT_EXPRESSION() || LIS('+') || LIS('-') )

// FOLLOW(multiplicative-expression)=         {$ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= , << , >> , + , - , * , / , % ,  }
#define FOLLOW_MULTIPLICATIVE_EXPRESSION() \
    ( FOLLOW_ADDITIVE_EXPRESSION() || LIS('*') || LIS('/') || LIS('%') )

// FOLLOW(unary-expression)={AssignOperator , $ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= , << , >> , + , - , * , / , % ,  }
#define FOLLOW_UNARY_EXPRESSION() \
    ( F_ASSIGNMENT_OPERATOR() || FOLLOW_MULTIPLICATIVE_EXPRESSION() )

// FOLLOW(cast-expression)= {AssignOperator , $ , , , : , ] , ) , ? , || , && , | , ^ , & , == , != , < , > , <= , >= , << , >> , + , - , * , / , % ,  }
#define FOLLOW_CAST_EXPRESSION() FOLLOW_UNARY_EXPRESSION()

// FOLLOW(argument-expression-list)={, , ) ,  }
// TODO: Consider if this is a part of FOLLOW(expression)
#define FOLLOW_ARGUMENT_EXPRESSION_LIST() \
    ( LIS(',') || LIS(')') )

// FOLLOW(unary-operator)={identifier , constant , string-literal , ( , ++ , -- , & , * , + , - , ~ , ! , sizeof ,  }
#define FOLLOW_UNARY_OPERATOR() FIRST_EXPRESSION()

////////////////////////   declaration   //////////////////////////////

#define FIRST_DECLARATION FIRST_DECLARATION_SPECIFIERS

#define FIRST_DECLARATION_SPECIFIERS() \
    ( FIRST_STORAGE_CLASS_SPECIFIER() || FIRST_TYPE_SPECIFIER() || FIRST_TYPE_QUALIFIER() || FIRST_FUNCTION_SPECIFIER() )

#define FIRST_STORAGE_CLASS_SPECIFIER() \
    ( ISTAG(Typedef) || ISTAG(Extern) || ISTAG(Static) || ISTAG(KThread_local) || ISTAG(Auto) || ISTAG(Register) )

#define FIRST_TYPE_SPECIFIER() \
    ( ISTAG(Void) || ISTAG(Char) || ISTAG(Short) || ISTAG(Int) || ISTAG(Long) || ISTAG(Float) || ISTAG(Double) || ISTAG(Signed) || \
    	ISTAG(Unsigned) || ISTAG(KBool) || ISTAG(KComplex) || ISTAG(Identifier) || FIRST_STRUCT_OR_UNION_SPECIFIER() || FIRST_ENUM_SPECIFIER() )
#define FIRST_STRUCT_OR_UNION_SPECIFIER() ( ISTAG(Struct) || ISTAG(Union) )
#define FIRST_ENUM_SPECIFIER() ISTAG(Enum)

#define FIRST_TYPE_QUALIFIER() \
    ( ISTAG(Const) || ISTAG(Restrict) || ISTAG(Volatile) || ISTAG(KAtomic) )

#define FIRST_FUNCTION_SPECIFIER() \
    ( ISTAG(Inline) )

#define FIRST_INIT_DECLARATOR_LIST FIRST_INIT_DECLARATOR
#define FIRST_INIT_DECLARATOR FIRST_DECLARATOR
#define FIRST_DECLARATOR() ( FIRST_POINTER() || FIRST_DIRECT_DECLARATOR() )
#define FIRST_POINTER() ( LIS('*') )
#define FIRST_DIRECT_DECLARATOR() ( ISTAG(Identifier) || LIS('(') )

#define FIRST_INITIALIZER() ( FIRST_EXPRESSION() || LIS('{') )
#define FIRST_INITIALIZER_LIST() ( FIRST_DESIGNATION() || FIRST_INITIALIZER() )
#define FIRST_DESIGNATION FIRST_DESIGNATOR_LIST
#define FIRST_DESIGNATOR_LIST FIRST_DESIGNATOR
#define FIRST_DESIGNATOR() ( LIS('[') || LIS('.') )

#define FIRST_STRUCT_DECLARATION FIRST_SPECIFIER_QUALIFIER_LIST
#define FIRST_SPECIFIER_QUALIFIER_LIST() ( FIRST_TYPE_QUALIFIER() || FIRST_TYPE_SPECIFIER() )

#define FIRST_STRUCT_DECLARATION_LIST FIRST_STRUCT_DECLARATION
#define FIRST_STRUCT_DECLARATOR_LIST FIRST_STRUCT_DECLARATOR
#define FIRST_STRUCT_DECLARATOR() ( FIRST_DECLARATOR() || LIS(':') )
#define FIRST_DIRECT_DECLARATOR() ( ISTAG(Identifier) || LIS('(') )
#define FIRST_ENUMERATOR_LIST FIRST_ENUMERATOR
#define FIRST_ENUMERATOR() ( ISTAG(Identifier) )
#define FIRST_DIRECT_ABSTRACT_DECLARATOR() ( LIS('(') || LIS('[') )

#define FIRST_TYPE_QUALIFIER_LIST FIRST_TYPE_QUALIFIER
#define FIRST_PARAMETER_TYPE_LIST FIRST_PARAMETER_LIST
#define FIRST_PARAMETER_LIST FIRST_PARAMETER_DECLARATION
#define FIRST_PARAMETER_DECLARATION FIRST_DECLARATION_SPECIFIERS
#define FIRST_IDENTIFIER_LIST() ( ISTAG(Identifier) )
#define FIRST_TYPE_NAME FIRST_SPECIFIER_QUALIFIER_LIST
#define FIRST_ANSTRACT_DECLARATOR() ( FIRST_POINTER() || FIRST_DIRECT_ABSTRACT_DECLARATOR() )

#define FOLLOW_PARAMETER_DECLARATION() ( LIS(')') || LIS(',') )

////////////////////////   statements   //////////////////////////////
#define FIRST_STATEMENT() ( FIRST_LABELED_STATEMENT() || FIRST_COMPOUND_STATEMENT() || FIRST_EXPRSSION_STATEMENT() || FIRST_SELECTION_STATEMENT() ||\
    FIRST_ITERATION_STATEMENT() || FIRST_JUMP_STATEMENT() )
#define FIRST_LABELED_STATEMENT() ( ISTAG(Identifier) || ISTAG(Case) || ISTAG(Default) )
#define FIRST_COMPOUND_STATEMENT() ( LIS('{') )
#define FIRST_BLOCK_ITEM_LIST FIRST_BLOCK_ITEM
#define FIRST_BLOCK_ITEM() ( FIRST_DECLARATION() || FIRST_STATEMENT() )
#define FIRST_EXPRSSION_STATEMENT() ( FIRST_EXPRESSION() || LIS(';') )
#define FIRST_SELECTION_STATEMENT() ( ISTAG(If) || ISTAG(Switch) )
#define FIRST_ITERATION_STATEMENT() ( ISTAG(While) || ISTAG(Do) || ISTAG(For) )
#define FIRST_JUMP_STATEMENT() ( ISTAG(Goto) || ISTAG(Continue) || ISTAG(Break) || ISTAG(Return) )

/////////////////////  external definiations /////////////////////////
#define FIRST_TRANSLATION_UNIT FIRST_EXTERNAL_DECLARATION
#define FIRST_EXTERNAL_DECLARATION() ( FIRST_FUNCTION_DEFINITION() || FIRST_DECLARATION() )
#define FIRST_FUNCTION_DEFINITION FIRST_DECLARATION_SPECIFIERS
#define FIRST_DECLARATION_LIST FIRST_DECLARATION 

#endif