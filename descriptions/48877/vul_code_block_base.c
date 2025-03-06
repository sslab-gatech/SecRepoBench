#ifdef MRB_USE_BIGINT
  if (mrb_bigint_p(v2)) {
    return mrb_bint_cmp(mrb, v1, v2);
  }
#endif

#ifdef MRB_NO_FLOAT
  mrb_int x, y;
#else
  mrb_float x, y;
#endif

#ifdef MRB_NO_FLOAT
  x = mrb_integer(v1);
#else
  x = mrb_as_float(mrb, v1);
#endif