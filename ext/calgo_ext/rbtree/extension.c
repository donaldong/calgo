#include "extconf.h"
#include "rb-tree.h"
#include "ruby.h"

static void
_rb_free(void *tree)
{
  rb_tree_free((RBTree *)tree);
}

static void
_rb_mark_value(RBTreeNode *subtree)
{
	if (subtree == NULL) {
		return;
	}

	_rb_mark_value(subtree->children[RB_TREE_NODE_LEFT]);

  rb_gc_mark((VALUE) subtree->key);
  rb_gc_mark((VALUE) subtree->value);

	_rb_mark_value(subtree->children[RB_TREE_NODE_RIGHT]);
}

static void
_rb_mark(void *tree)
{
  _rb_mark_value(((RBTree *)tree)->root_node);
}

static const rb_data_type_t rb_tree_type = {
  "rbtree",
  {_rb_mark, _rb_free, NULL},
};

static int
_rb_compare_func(VALUE value1, VALUE value2)
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
_rb_alloc(VALUE self)
{
  RBTree *tree = rb_tree_new((RBTreeCompareFunc) &_rb_compare_func);

  return TypedData_Wrap_Struct(self, &rb_tree_type, tree);
}

static VALUE
_rb_clear(VALUE self)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);
	rb_tree_free_subtree(tree->root_node);

	tree->root_node = NULL;
	tree->num_nodes = 0;

  return Qnil;
}

static VALUE
_rb_insert(VALUE self, VALUE key, VALUE node)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);
  rb_tree_insert(tree, (void *) key, (void *) node);

  return Qnil;
}

static VALUE
_rb_size(VALUE self)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);

  return UINT2NUM(tree->num_nodes);
}

static VALUE
_rb_include(VALUE self, VALUE key)
{
  RBTree *tree;
  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);

  if (rb_tree_lookup_node(tree, (void *) key)) {
    return Qtrue;
  }

  return Qfalse;
}

static VALUE
_rb_delete(VALUE self, VALUE key)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);

  if (rb_tree_remove(tree, (void *) key)) {
    return Qtrue;
  }

  return Qfalse;
}

static RBTreeNode *
_rb_left(RBTree *tree, RBTreeNode *node, VALUE lower_bound)
{
	if (node == NULL) {
		return NULL;
	}

  if (!NIL_P(lower_bound)) {
	  int diff = tree->compare_func(node->key, (void *) lower_bound);

    if (diff < 0) {
      return NULL;
    }
  }

	return node->children[RB_TREE_NODE_LEFT];
}

static RBTreeNode *
_rb_right(RBTree *tree, RBTreeNode *node, VALUE upper_bound, VALUE include_upper_bound)
{
	if (node == NULL) {
		return NULL;
	}

  if (!NIL_P(upper_bound)) {
	  int diff = tree->compare_func((void *) upper_bound, node->key);

    if (RTEST(include_upper_bound)) {
      if (diff < 0) {
        return NULL;
      }
    }
    else {
      if (diff <= 0) {
        return NULL;
      }
    }
  }

	return node->children[RB_TREE_NODE_RIGHT];
}

static void
_yield_if_valid(RBTree *tree, RBTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
  int diff_left = 0, diff_right = 1;

  if (!NIL_P(lower_bound)) {
    diff_left = tree->compare_func(node->key, (void *) lower_bound);
  }

  if (!NIL_P(upper_bound)) {
    diff_right = tree->compare_func((void *) upper_bound, node->key);
  }

  if (RTEST(include_upper_bound)) {
    if (diff_left >= 0 && diff_right >= 0) {
      rb_funcall(generator, rb_intern("yield"), 2, (VALUE) node->key, (VALUE) node->value);
    }
  }
  else {
    if (diff_left >= 0 && diff_right > 0) {
      rb_funcall(generator, rb_intern("yield"), 2, (VALUE) node->key, (VALUE) node->value);
    }
  }
}

static void
_rb_yield_preorder(RBTree *tree, RBTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
	if (node == NULL) {
    return;
  }

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound, include_upper_bound);

  _rb_yield_preorder(
      tree,
      _rb_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound, include_upper_bound);

  _rb_yield_preorder(
      tree,
      _rb_right(tree, node, upper_bound, include_upper_bound),
      generator, lower_bound, upper_bound, include_upper_bound);
}

static void
_rb_yield_inorder(RBTree *tree, RBTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
	if (node == NULL) {
    return;
  }

  _rb_yield_preorder(
      tree,
      _rb_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound, include_upper_bound);

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound, include_upper_bound);

  _rb_yield_preorder(
      tree,
      _rb_right(tree, node, upper_bound, include_upper_bound),
      generator, lower_bound, upper_bound, include_upper_bound);
}

static void
_rb_yield_postorder(RBTree *tree, RBTreeNode *node, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
	if (node == NULL) {
    return;
  }

  _rb_yield_preorder(
      tree,
      _rb_left(tree, node, lower_bound),
      generator, lower_bound, upper_bound, include_upper_bound);

  _rb_yield_preorder(
      tree,
      _rb_right(tree, node, upper_bound, include_upper_bound),
      generator, lower_bound, upper_bound, include_upper_bound);

  _yield_if_valid(tree, node, generator, lower_bound, upper_bound, include_upper_bound);
}

static VALUE
_yield_preorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);
  _rb_yield_preorder(tree, tree->root_node, generator, lower_bound, upper_bound, include_upper_bound);

  return Qnil;
}

static VALUE
_yield_inorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);
  _rb_yield_inorder(tree, tree->root_node, generator, lower_bound, upper_bound, include_upper_bound);

  return Qnil;
}

static VALUE
_yield_postorder(VALUE self, VALUE generator, VALUE lower_bound, VALUE upper_bound, VALUE include_upper_bound)
{
  RBTree *tree;

  TypedData_Get_Struct(self, RBTree, &rb_tree_type, tree);
  _rb_yield_postorder(tree, tree->root_node, generator, lower_bound, upper_bound, include_upper_bound);

  return Qnil;
}

void
Init_rbtree(void)
{
  VALUE cCalgo = rb_define_module("Calgo");
  VALUE cRedBlackTree = rb_define_class_under(cCalgo, "RedBlackTree", rb_cObject);

  rb_define_alloc_func(cRedBlackTree, _rb_alloc);
  rb_define_method(cRedBlackTree, "clear", _rb_clear, 0);
  rb_define_method(cRedBlackTree, "insert", _rb_insert, 2);
  rb_define_method(cRedBlackTree, "size", _rb_size, 0);
  rb_define_method(cRedBlackTree, "include?", _rb_include, 1);
  rb_define_method(cRedBlackTree, "delete", _rb_delete, 1);
  rb_define_method(cRedBlackTree, "_yield_preorder", _yield_preorder, 4);
  rb_define_method(cRedBlackTree, "_yield_inorder", _yield_inorder, 4);
  rb_define_method(cRedBlackTree, "_yield_postorder", _yield_postorder, 4);
}
