static int
unpack_bstr(mrb_state *mrb, const void *src, int slen, mrb_value ary, int count, unsigned int flags)
{
  // <MASK>
  dst = mrb_str_resize(mrb, dst, (mrb_int)(dptr - dptr0));
  mrb_ary_push(mrb, ary, dst);
  return (int)(sptr - sptr0);
}