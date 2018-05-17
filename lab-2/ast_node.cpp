#include "ast_node.hpp"

ast_node::ast_node()
{
  type = ast_undefined;
}

ast_node::ast_node(ast_node_type t, ast_node child)
{
  type = t;
  children.push_back(child);
}

ast_node::ast_node(ast_node_type t, ast_node child, ast_node child2)
{
  type = t;
  children.push_back(child);
  children.push_back(child2);
}

ast_node::ast_node(ast_node_type t, ast_node child, ast_node child2, ast_node child3)
{
  type = t;
  children.push_back(child);
  children.push_back(child2);
  children.push_back(child3);
}

ast_node::ast_node(ast_node_type t)
{
  type = t;
}
