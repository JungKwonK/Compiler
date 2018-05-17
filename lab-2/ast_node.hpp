#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <vector>

enum ast_node_type
{
   ast_undefined,
   ast_string_literal,
   ast_identifier,
   ast_import_item,
   ast_import_statement,
   ast_package_statement,
   ast_import,
   ast_package,
   ast_root
};

class ast_node
{
public:
	ast_node_type type;

	std::vector<ast_node> children;

	ast_node();
	ast_node(ast_node_type t);

	ast_node(ast_node_type t, ast_node child);

	ast_node(ast_node_type t, ast_node child, ast_node child2);

	ast_node(ast_node_type t, ast_node child, ast_node child2, ast_node child3);

};

#endif
