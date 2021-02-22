#include "extconf.h"
#include "kdtree.h"
#include "ruby.h"

static void
_kd_free(struct kdtree *tree)
{
  kd_free(tree);
}

static const rb_data_type_t kd_tree_type = {
  "kdtree",
  {NULL, _kd_free, NULL},
};

static VALUE
_kd_alloc(VALUE self)
{
	struct kdtree *tree;

	if (!(tree = malloc(sizeof *tree))) {
		return Qnil;
	}

  return TypedData_Make_Struct(self, struct kdtree, &kd_tree_type, tree);
}

static VALUE
_kd_m_initialize(VALUE self, VALUE val)
{
	struct kdtree *tree;

  TypedData_Get_Struct(self, struct kdtree, &kd_tree_type, tree);

	tree->dim = NUM2INT(val);
	tree->root = 0;
	tree->destr = 0;
	tree->rect = 0;

  return self;
}

static inline double *
_position_ary(VALUE position, int len)
{
  int i;
  double *pos = malloc(len * sizeof *pos);
  for (i = 0; i < len; i++) {
    pos[i] = NUM2DBL(rb_ary_entry(position, i));
  }
  return pos;
}

static VALUE
_kd_clear(VALUE self)
{
	struct kdtree *tree;

  TypedData_Get_Struct(self, struct kdtree, &kd_tree_type, tree);
  kd_clear(tree);

  return Qnil;
}

static VALUE
_kd_insert(VALUE self, VALUE node, VALUE position)
{
	struct kdtree *tree;
  double *pos;


  TypedData_Get_Struct(self, struct kdtree, &kd_tree_type, tree);
  pos = _position_ary(position, tree->dim);
  kd_insert(tree, pos, (void *) node);

  free(pos);
  return Qnil;
}

static VALUE
_kd_nearest(VALUE self, VALUE range, VALUE position)
{
	struct kdtree *tree;
  struct kdres *res;
  void *item;
  double *pos;
  VALUE ary;

  TypedData_Get_Struct(self, struct kdtree, &kd_tree_type, tree);
  pos = _position_ary(position, tree->dim);

  if (NIL_P(range)) {
    res = kd_nearest(tree, pos);
  }
  else {
    res = kd_nearest_range(tree, pos, NUM2DBL(range));
  }

  ary = rb_ary_new();
  while (!kd_res_end(res)) {
    item = kd_res_item_data(res);
    rb_ary_push(ary, (VALUE) item);
    kd_res_next(res);
  }

  free(pos);
  kd_res_free(res);
  return ary;
}

/*
 * class Calgo::KdTree
 *   def initialize(dimension:); end
 *   def clear; end
 *   def insert(node); end
 * end
 */
void
Init_extension(void)
{
  VALUE cCalgo = rb_define_module("Calgo");
  VALUE cKdTree = rb_define_class_under(cCalgo, "KdTree", rb_cObject);

  rb_define_alloc_func(cKdTree, _kd_alloc);
  rb_define_method(cKdTree, "_initialize", _kd_m_initialize, 1);
  rb_define_method(cKdTree, "clear", _kd_clear, 0);
  rb_define_method(cKdTree, "_insert", _kd_insert, 2);
  rb_define_method(cKdTree, "_nearest", _kd_nearest, 2);
}
