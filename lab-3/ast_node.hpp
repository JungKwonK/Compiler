#ifndef AST_NODE_HPP
#define AST_NODE_HPP

class ast_node
{
};

class ast_pkg_decl : public ast_node
{
public:
	ast_pkg_decl(ast_str_lit name);
};

class ast_imp_decl : public ast_node
{
public:
	ast_imp_decl *next;

	ast_imp_decl(ast_imp_spec imp_specs);
};

class ast_imp_spec : public ast_node
{
public:
	ast_imp_spec *next;

	ast_imp_spec(ast_str_lit path);
	ast_imp_spec(ast_ident name, ast_str_lit path);
};

class ast_block : public ast_node
{
public:
	ast_block(ast_stmt statements);
	ast_block();
};

class ast_stmt : public ast_node
{
public:
	ast_stmt *next;

	ast_stmt(ast_func_decl func_decl);
	ast_stmt(ast_var_decl var_decl);
	ast_stmt(ast_expr expr);
};

class ast_func_decl : public ast_stmt
{
public:
	ast_func_decl(ast_ident name, ast_var_decl args, ast_ident return_type, ast_block block);
};

class ast_var_decl : public ast_stmt
{
public:
	ast_var_decl *next;

	ast_var_assign(ast_ident name, ast_ident var_type);
	ast_var_assign(ast_ident name, ast_expr value);
	ast_var_assign(ast_ident name, ast_ident var_type, ast_expr value);
};

class ast_expr : public ast_stmt
{
};

class ast_ident : public ast_expr
{
public:
	ast_ident(std::string name);
};

class ast_int_lit : public ast_expr
{
public:
	ast_int_lit(std::string value);
};

class ast_str_lit : public ast_expr
{
public:
	ast_int_lit(std::string value);
};

class ast_operation : public ast_expr
{
public:
	ast_operation(ast_expr lhs, std::string binary_op, ast_expr rhs);
	ast_operation(std::string unary_op, ast_expr operand);
};

class ast_func_call : public ast_expr
{
public:
	ast_func_call(ast_ident name);
	ast_func_call(ast_ident name, ast_expr args);

};

class ast_var_assign : public ast_expr
{
public:
	ast_var_assign(ast_ident name, ast_ident value);
};

class ast_root : public ast_node
{
public:
	ast_root(ast_pkg_decl package, ast_imp_decl imports, ast_stmt statements);

	ast_root(ast_pkg_decl package, ast_stmt statements);
}

#endif
