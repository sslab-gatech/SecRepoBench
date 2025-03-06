static mrb_int
cmpnum(mrb_state *mrb, mrb_value v1, mrb_value v2)
{
// Compare two numerical values, v1 and v2, in the context of a Ruby-like environment.
// Check if either value is a big integer and handle this case separately using a specific comparison function for big integers.
// Determine the type of v1 and convert it into a numerical form suitable for comparison.
// Decide the type of variables x and y based on whether floating-point support is enabled, using integers if floating-point is disabled.
// Store the converted numerical value of v1 into the variable x for further comparison with v2.
// <MASK>
  switch (mrb_type(v2)) {
  case MRB_TT_INTEGER:
#ifdef MRB_NO_FLOAT
    y = mrb_integer(v2);
#else
    y = (mrb_float)mrb_integer(v2);
#endif
    break;
#ifndef MRB_NO_FLOAT
  case MRB_TT_FLOAT:
    y = mrb_float(v2);
    break;
#ifdef MRB_USE_RATIONAL
  case MRB_TT_RATIONAL:
    y = mrb_as_float(mrb, v2);
    break;
#endif
#endif
  default:
    return -2;
  }
  if (x > y)
    return 1;
  else {
    if (x < y)
      return -1;
    return 0;
  }
}