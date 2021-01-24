#ifndef ast_hpp
#define ast_hpp

#include "ast/ast_program.hpp"
#include "ast/ast_programTop.hpp"
#include "ast/ast_declaration.hpp"
#include "ast/ast_function.hpp"
#include "ast/ast_sequence.hpp"
#include "ast/ast_expression.hpp"
#include "ast/ast_primitives.hpp"
#include "ast/ast_operators.hpp"
#include "ast/ast_unary.hpp"
#include "ast/ast_array.hpp"

#include "ast/context.hpp"


extern const Program *parseAST();

#endif
