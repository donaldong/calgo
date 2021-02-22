#include "extconf.h"
#include "avl-tree.h"
#include "ruby.h"

static void
_avl_free(void *tree)
{
  avl_tree_free((AVLTree *)tree);
}

static void
_avl_mark_value(AVLTreeNode *subtree)
{
	if (subtree == NULL) {
		return;
	}

	_avl_mark_value(subtree->children[AVL_TREE_NODE_LEFT]);

  rb_gc_mark((VALUE) subtree->key);
  rb_gc_mark((VALUE) subtree->value);

	_avl_mark_value(subtree->children[AVL_TREE_NODE_RIGHT]);
}

static void
_avl_mark(void *tree)
{
  _avl_mark_value(((AVLTree *)tree)->root_node);
}

static const rb_data_type_t avl_tree_type = {
  "avltree",
  {_avl_mark, _avl_free, NULL},
};

static int
_avl_compare_func(AVLTreeValue value1, AVLTreeValue value2)
{
  VALUE eql = rb_funcall(value1, rb_intern("=="), 1, value2);

  if (RTEST(eql)) {
    return 0;
  }

  VALUE lt = rb_funcall(value1, rb_intern("<"), 1, value2);
  if (RTEST(lt)) {
    return -1;
  }

  return 1;
}

static VALUE
_avl_alloc(VALUE self)
{
  AVLTree *tree = avl_tree_new(&_avl_compare_func);

  return TypedData_Wrap_Struct(self, &avl_tree_type, tree);
}

static VALUE
_avl_clear(VALUE self)
{
  AVLTree *tree;

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);
	avl_tree_free_subtree(tree, tree->root_node);

	tree->root_node = NULL;
	tree->num_nodes = 0;

  return Qnil;
}

static VALUE
_avl_insert(VALUE self, VALUE node)
{
  AVLTree *tree;
  VALUE key = rb_funcall(self, rb_intern("_key"), 1, node);

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);
  avl_tree_insert(tree, key, node);

  return Qnil;
}

static VALUE
_avl_size(VALUE self)
{
  AVLTree *tree;

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);

  return UINT2NUM(tree->num_nodes);
}

static VALUE
_avl_include(VALUE self, VALUE node)
{
  AVLTree *tree;
  VALUE key = rb_funcall(self, rb_intern("_key"), 1, node);

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);

  if (avl_tree_lookup_node(tree, key)) {
    return Qtrue;
  }

  return Qfalse;
}

static VALUE
_avl_delete(VALUE self, VALUE node)
{
  AVLTree *tree;
  VALUE key = rb_funcall(self, rb_intern("_key"), 1, node);

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);

  AVLTreeNode *node_to_delete = avl_tree_lookup_node(tree, key);
  if (node_to_delete) {
    avl_tree_remove_node(tree, node_to_delete);
    return Qtrue;
  }

  return Qfalse;
}

static AVLTreeNode *
_avl_left(AVLTree *tree, AVLTreeNode *node, VALUE lower_bound)
{
	if (node == NULL) {
		return NULL;
	}

  if (!NIL_P(lower_bound)) {
	  int diff = tree->compare_func(node->key, lower_bound);

    if (diff < 0) {
      return NULL;
    }
  }

	return node->children[AVL_TREE_NODE_LEFT];
}

static AVLTreeNode *
_avl_right(AVLTree *tree, AVLTreeNode *node, VALUE upper_bound)
{
	if (node == NULL) {
		return NULL;
	}

  if (!NIL_P(upper_bound)) {
	  int diff = tree->compare_func(upper_bound, node->key);

    if (diff <= 0) {
      return NULL;
    }
  }

	return node->children[AVL_TREE_NODE_RIGHT];
}

static void
_yield_if_valid(AVLTree *tree, AVLTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
  int diff_left = 0, diff_right = 1;

  if (!NIL_P(lower_bound)) {
    diff_left = tree->compare_func(node->key, lower_bound);
  }

  if (!NIL_P(upper_bound)) {
    diff_right = tree->compare_func(upper_bound, node->key);
  }

  if (diff_left >= 0 && diff_right > 0) {
    rb_funcall(generator, rb_intern("yield"), 1, node->value);
  }
}

static void
_avl_yield_preorder(AVLTree *tree, AVLTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
	if (node == NULL) {
    return;
  }

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound);

  _avl_yield_preorder(
      tree,
      _avl_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound);

  _avl_yield_preorder(
      tree,
      _avl_right(tree, node, upper_bound),
      generator, lower_bound, upper_bound);
}

static void
_avl_yield_inorder(AVLTree *tree, AVLTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
	if (node == NULL) {
    return;
  }

  _avl_yield_preorder(
      tree,
      _avl_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound);

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound);

  _avl_yield_preorder(
      tree,
      _avl_right(tree, node, upper_bound),
      generator, lower_bound, upper_bound);
}

static void
_avl_yield_postorder(AVLTree *tree, AVLTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
	if (node == NULL) {
    return;
  }

  _avl_yield_preorder(
      tree,
      _avl_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound);

  _avl_yield_preorder(
      tree,
      _avl_right(tree, node, upper_bound),
      generator, lower_bound, upper_bound);

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound);
}

static void
_yield_preorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
  AVLTree *tree;

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);
  _avl_yield_preorder(tree, tree->root_node, generator, lower_bound, upper_bound);
}

static void
_yield_inorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
  AVLTree *tree;

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);
  _avl_yield_inorder(tree, tree->root_node, generator, lower_bound, upper_bound);
}

static void
_yield_postorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound)
{
  AVLTree *tree;

  TypedData_Get_Struct(self, AVLTree, &avl_tree_type, tree);
  _avl_yield_postorder(tree, tree->root_node, generator, lower_bound, upper_bound);
}

void
Init_avltree(void)
{
  VALUE cCalgo = rb_define_module("Calgo");
  VALUE cAvlTree = rb_define_class_under(cCalgo, "AvlTree", rb_cObject);

  rb_define_alloc_func(cAvlTree, _avl_alloc);
  rb_define_method(cAvlTree, "clear", _avl_clear, 0);
  rb_define_method(cAvlTree, "insert", _avl_insert, 1);
  rb_define_method(cAvlTree, "size", _avl_size, 0);
  rb_define_method(cAvlTree, "include?", _avl_include, 1);
  rb_define_method(cAvlTree, "delete", _avl_delete, 1);
  rb_define_method(cAvlTree, "_yield_preorder", _yield_preorder, 3);
  rb_define_method(cAvlTree, "_yield_inorder", _yield_inorder, 3);
  rb_define_method(cAvlTree, "_yield_postorder", _yield_postorder, 3);
}
