
  mrb->c->ci->mid = MRB_SYM(inspect);
  mrb_value ret = mrb_str_new_lit(mrb, "[");
  int ai = mrb_gc_arena_save(mrb);
  if (mrb_inspect_recursive_p(mrb, self)) {
    mrb_str_cat_lit(mrb, ret, "...]");
    return ret;
  }
  mrb_int len = RARRAY_LEN(self);
  for (mrb_int i=0; i<len; i++) {
    if (i>0) mrb_str_cat_lit(mrb, ret, ", ");
    mrb_str_cat_str(mrb, ret, mrb_inspect(mrb, mrb_ary_ref(mrb, self, i)));
    mrb_gc_arena_restore(mrb, ai);
  }
  mrb_str_cat_lit(mrb, ret, "]");

  return ret;
