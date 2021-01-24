%code requires{
	#include "ast.hpp"
	#include <cassert>
	extern const Program *g_root; // A way of getting the AST out

	//some includes
	int yylex(void);
  void yyerror(const char *);
}

%union{
  double number;
  Program *expr;
	DeclarationList *declaralation;
  std::string *string;
}
%define parse.error verbose

%token T_DIVIDE T_PLUS T_TIMES T_MINUS
%token T_EQUAL T_NOT_EQUAL T_LT T_MT T_LET T_MET
%token T_LAND T_LOR T_NOT
%token T_AND T_OR T_XOR
%token T_MODULO
%token T_ASSIGN
%token T_LSHIFT T_RSHIFT
%token T_LBRACKET T_RBRACKET
%token T_LCURLYBRACKET T_RCURLYBRACKET
%token T_LSQUAREBRACKET T_RSQUAREBRACKET
%token T_COMMA T_SEMICOLON T_COLON
%token T_IF T_ELSE T_WHILE T_FOR T_SWITCH T_CASE T_DEFAULT
%token T_INCR T_DECR
%token T_RETURN T_BREAK T_CONTINUE
%token T_ENUM
%token T_INT T_VOID T_FLOAT T_DOUBLE
%token T_TYPE
%token T_NUMBER T_ID


%type <expr> EXPR PROGRAM EXT_DECL POSTFIX_EXPR POSTFIX_EXPR2 ARG_EXPR_LIST ASSIGN_EXPR
%type<expr> ADD_EXPR MULT_EXPR CAST_EXPR UNARY_EXPR COND_EXPR LOG_OR_EXPR LOG_AND_EXPR PRIMARY_EXPR
%type <expr> INIT_DECLARATOR INIT_DECLARATOR_LIST DECLARATION_LIST DECLARATION DIRECT_DECLARATOR
%type <expr> PARAM_DECL PARAM PARAM_LIST FUNC_DEF
%type <expr> INITIALIZER INITIALIZER_LIST
%type <expr> SEQUENCE STATEMENT EXPR_STATEMENT SELEC_STATEMENT COMP_STATEMENT COMP_STATEMENT2 ITERATION_STATEMENT LABELED_STATEMENT JUMP_STATEMENT
%type <expr> ENUMERATOR ENUMERATOR_LIST ENUM_SPECIFIER
%type <expr> SHIFT_EXPR RELATIONAL_EXPR EQUALITY_EXPR AND_EXPR EXCLUSIVE_OR_EXPR INCLUSIVE_OR_EXPR
%type <number> T_NUMBER
%type <string> T_ID UNARY_OPERATOR DECLARATOR_SPECIFIER T_TIMES T_PLUS T_AND T_MINUS T_NOT
%type <string> DECLARATION_SPECIFIER

%start ROOT


%%

ROOT : PROGRAM { g_root = $1; }

PROGRAM : EXT_DECL {$$ = new ProgramTop($1);}
				| PROGRAM EXT_DECL {$$->push($2);}


EXT_DECL : DECLARATION {$$=$1;}
				 | FUNC_DEF {$$ = $1;}
				 | ENUM_SPECIFIER {$$ = $1;}

//Functions
FUNC_DEF : DECLARATION_SPECIFIER DIRECT_DECLARATOR COMP_STATEMENT { $$ = new Function($2->getID(),$2->getParams(),$3);}
				 //| T_VOID T_ID T_LBRACKET PARAM_LIST T_RBRACKET COMP_STATEMENT { $$ = new Function(*$2,$4,$6);} //EDIT THIS LATER

//params
PARAM_LIST : PARAM_DECL	{$$= new ParamList($1);}
 					 | PARAM_LIST T_COMMA PARAM_DECL {$$->push($3);}

PARAM_DECL : DECLARATION_SPECIFIER DIRECT_DECLARATOR { $$ = new Parameter(*$1,$2);}

//Declarations
DECLARATION_LIST : DECLARATION { $$ = new DeclarationList($1); }
								 | DECLARATION_LIST DECLARATION {$$->push($2); }

DECLARATION : DECLARATION_SPECIFIER INIT_DECLARATOR_LIST T_SEMICOLON {$$ = $2;} //get type
						| ENUM_SPECIFIER		{$$=$1;}

DECLARATION_SPECIFIER : T_INT { $$ = new std::string("int");}
											| T_VOID { $$ = new std::string("void");}
											| T_FLOAT { $$ = new std::string("float");}
											| T_DOUBLE { $$ = new std::string("double");}

INIT_DECLARATOR_LIST : INIT_DECLARATOR {$$ = new initDeclarationList($1);}
										 | INIT_DECLARATOR_LIST T_COMMA INIT_DECLARATOR { $$->push($3);}

INIT_DECLARATOR : DIRECT_DECLARATOR {$$ = $1;}
								| DIRECT_DECLARATOR T_ASSIGN INITIALIZER {$$ = $1; $$->setInitializer($3);}

DIRECT_DECLARATOR : T_ID {$$ = new IdentifierDeclaration(*$1); delete $1;}
									| DIRECT_DECLARATOR T_LSQUAREBRACKET T_RSQUAREBRACKET { $$ = new Array($1); }
									| DIRECT_DECLARATOR T_LSQUAREBRACKET COND_EXPR T_RSQUAREBRACKET { $$ = new Array($1, $3); }
									| DIRECT_DECLARATOR T_LBRACKET T_RBRACKET { $$=$1; $$->addParams(new ParamList());$$->setFuncDec();}
									| DIRECT_DECLARATOR T_LBRACKET PARAM_LIST T_RBRACKET { $$=$1; $$->addParams($3);$$->setFuncDec(); }



INITIALIZER_LIST: INITIALIZER {$$= new InitializerList($1);}
					 			| INITIALIZER_LIST T_COMMA INITIALIZER { $$->push($3);}

INITIALIZER: ASSIGN_EXPR { $$ = new Initializer($1);}
					 | T_LCURLYBRACKET INITIALIZER_LIST T_RCURLYBRACKET { $$ = new Initializer($2);}
					 | T_LCURLYBRACKET INITIALIZER_LIST T_COMMA T_RCURLYBRACKET { $$ = new Initializer($2);}

//Enums
ENUM_SPECIFIER: T_ENUM T_ID T_SEMICOLON {$$ = new Enum(*$2);} //I'm confused why anyone would write this
						//	| T_ENUM T_LCURLYBRACKET ENUMERATOR_LIST T_RCURLYBRACKET T_SEMICOLON { $$ = new Enum($3); } //not sure what to do here
							| T_ENUM T_ID T_LCURLYBRACKET ENUMERATOR_LIST T_RCURLYBRACKET T_SEMICOLON { $$ = new Enum(*$2,$4); }

ENUMERATOR_LIST: ENUMERATOR { $$ = new EnumList($1);} //enumerator
							 | ENUMERATOR_LIST T_COMMA ENUMERATOR { $$->push($3); }

ENUMERATOR : T_ID   {$$ = new EnumElement(*$1);}
					 | T_ID T_ASSIGN COND_EXPR {$$ = new EnumElement(*$1,$3);}

//Statements/sequences
SEQUENCE : STATEMENT          { $$ = new Sequence($1); }
         | SEQUENCE STATEMENT { $$->push($2); }


STATEMENT :  COMP_STATEMENT  {$$= new Scope($1);}
					|  EXPR_STATEMENT  {$$=$1;}
					|  SELEC_STATEMENT {$$=$1; }
					|  ITERATION_STATEMENT {$$=$1;}
					|  DECLARATION 		{$$=$1;}
					|  JUMP_STATEMENT {$$=$1;}
					|  ENUM_SPECIFIER {$$=$1;}
					|  LABELED_STATEMENT{$$=$1;}

COMP_STATEMENT : T_LCURLYBRACKET COMP_STATEMENT2 {$$ = $2;}

COMP_STATEMENT2 : T_RCURLYBRACKET {$$ = new CompoundStatement();}
								| SEQUENCE T_RCURLYBRACKET {$$ = new CompoundStatement($1); }
								| DECLARATION_LIST T_RCURLYBRACKET {$$ = new CompoundStatement($1);}
								| DECLARATION_LIST SEQUENCE T_RCURLYBRACKET {$$ = new CompoundStatement($1, $2);}

LABELED_STATEMENT: T_CASE COND_EXPR T_COLON STATEMENT {$$ = new CaseStatement($2, $4); $4->setIndividualFalse();}
								 | T_DEFAULT T_COLON STATEMENT { $$ = new CaseStatement($3); $3->setIndividualFalse();}

SELEC_STATEMENT : T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT { $$ = new SelectionStatement($3,$5); $5->setIndividualFalse();}
								| T_IF T_LBRACKET EXPR T_RBRACKET STATEMENT T_ELSE STATEMENT { $$ = new SelectionStatement($3,$5,$7);
																																							 $5->setIndividualFalse();
																																							 $7->setIndividualFalse();
																																							}
								| T_SWITCH T_LBRACKET EXPR T_RBRACKET STATEMENT { $$ = new SwitchStatement($3,$5); $5->setIndividualFalse();}


EXPR_STATEMENT : T_SEMICOLON { $$ = new ExpressionStatement(); }
							 |	EXPR T_SEMICOLON { $$ = new ExpressionStatement($1); $1->setIndividualFalse();}

ITERATION_STATEMENT : T_WHILE T_LBRACKET EXPR T_RBRACKET STATEMENT { $$ = new WhileLoop{$3, $5}; $5->setIndividualFalse();}
										| T_FOR T_LBRACKET EXPR T_SEMICOLON EXPR T_SEMICOLON EXPR T_RBRACKET STATEMENT { $$ = new ForLoop($3, $5, $7, $9);
																																																	   $9->setIndividualFalse();}

JUMP_STATEMENT : T_RETURN EXPR_STATEMENT {$$ = new RetStatement($2);}
							 | T_BREAK T_SEMICOLON { $$ = new BreakStatement(); }
							 | T_CONTINUE T_SEMICOLON { $$ = new ContinueStatement(); }
//Expressions
EXPR : ASSIGN_EXPR {$$ = $1;}

ASSIGN_EXPR : COND_EXPR {$$=$1;}
						| UNARY_EXPR T_ASSIGN ASSIGN_EXPR {$$ = new AssignExpression( $1, $3);}

COND_EXPR : LOG_OR_EXPR {$$=$1;}

LOG_OR_EXPR : LOG_AND_EXPR {$$=$1;}
						| LOG_OR_EXPR T_LOR LOG_AND_EXPR {$$ = new OrExpression( $1, $3 ); }

LOG_AND_EXPR : INCLUSIVE_OR_EXPR {$$=$1;}
						 | LOG_AND_EXPR T_LAND INCLUSIVE_OR_EXPR {$$ = new AndExpression( $1, $3 ); }

INCLUSIVE_OR_EXPR : EXCLUSIVE_OR_EXPR {$$=$1;}
									| INCLUSIVE_OR_EXPR T_OR EXCLUSIVE_OR_EXPR { $$ = new OrOperator($1,$3);}

EXCLUSIVE_OR_EXPR : AND_EXPR {$$=$1;}
				         | EXCLUSIVE_OR_EXPR T_XOR AND_EXPR { $$ = new XorOperator($1, $3);}

AND_EXPR : EQUALITY_EXPR {$$=$1;}
				 | AND_EXPR T_AND EQUALITY_EXPR { $$ = new AndOperator($1, $3);}

EQUALITY_EXPR : RELATIONAL_EXPR {$$=$1;}
							| EQUALITY_EXPR T_EQUAL RELATIONAL_EXPR { $$ = new EqOperator($1,$3);}
							|	EQUALITY_EXPR T_NOT_EQUAL RELATIONAL_EXPR { $$ = new NotEqualOperator($1,$3);}

RELATIONAL_EXPR : SHIFT_EXPR {$$=$1;}
								| RELATIONAL_EXPR T_LT ADD_EXPR  {$$ = new LTOperator( $1, $3 ); }
								| RELATIONAL_EXPR T_MT ADD_EXPR  {$$ = new MTOperator( $1, $3 ); }
								| RELATIONAL_EXPR T_LET ADD_EXPR  {$$ = new LesserThanEqualOperator( $1, $3 ); }
								| RELATIONAL_EXPR T_MET ADD_EXPR  {$$ = new GreaterThanEqualOperator( $1, $3 ); }

SHIFT_EXPR: ADD_EXPR { $$=$1; }
					| SHIFT_EXPR T_LSHIFT ADD_EXPR { $$ = new LeftShiftOperator($1, $3);}
					| SHIFT_EXPR T_RSHIFT ADD_EXPR { $$ = new RightShiftOperator($1, $3);}

ADD_EXPR : MULT_EXPR { $$ = $1; }
				 | ADD_EXPR T_PLUS MULT_EXPR {$$ = new AddOperator( $1, $3 ); }
				 | ADD_EXPR T_MINUS MULT_EXPR { $$ = new SubOperator( $1, $3 ); }

MULT_EXPR : CAST_EXPR { $$ = $1; }
          | MULT_EXPR T_TIMES CAST_EXPR { $$ = new MulOperator( $1, $3); }
					| MULT_EXPR T_DIVIDE CAST_EXPR { $$ = new DivOperator( $1, $3); }
					| MULT_EXPR T_MODULO CAST_EXPR { $$ = new ModOperator( $1, $3); }

CAST_EXPR : UNARY_EXPR { $$ = $1; }
					| T_LBRACKET T_TYPE T_LBRACKET CAST_EXPR { $$ = $4 ;}

UNARY_EXPR : POSTFIX_EXPR {$$=$1;}
					 | T_INCR UNARY_EXPR {$$=$2;}
					 | T_DECR UNARY_EXPR {$$=$2;}
					 | UNARY_OPERATOR CAST_EXPR {$$ = new UnaryOperatorExpression(*$1,$2);}
					 | T_MINUS CAST_EXPR {$$= new NegOperator($$=$2);}


UNARY_OPERATOR : T_PLUS {$$ = $1;}
							 | T_AND {$$ = $1;}
							 | T_MINUS {$$ = $1;}
							 | T_NOT {$$=$1;}

POSTFIX_EXPR : PRIMARY_EXPR {$$=$1;}
			 			 | POSTFIX_EXPR T_LBRACKET POSTFIX_EXPR2  { $$= new FunctionCall($1,$3);  }
						 | POSTFIX_EXPR	T_LSQUAREBRACKET EXPR T_RSQUAREBRACKET {$$ = new PostfixArray($1, $3); }
			 			 | POSTFIX_EXPR T_INCR {}
			 			 | POSTFIX_EXPR T_DECR {}

POSTFIX_EXPR2 : T_RBRACKET {$$= new Expression();}
			 				| ARG_EXPR_LIST T_RBRACKET {$$=$1;}

ARG_EXPR_LIST : ASSIGN_EXPR {$$= new ArgExprList($1);}
			 				| ARG_EXPR_LIST T_COMMA ASSIGN_EXPR {$$->push($3);}

PRIMARY_EXPR : T_ID { $$ = new Variable(*$1); }
						 | T_NUMBER { $$ = new Number($1); }
						 | T_LBRACKET EXPR T_RBRACKET { $$ = $2; }




%%

const Program *g_root; // Definition of variable (to match declaration earlier)

const Program  *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}
