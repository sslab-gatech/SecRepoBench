if (head < 0) {
    head += alen;
    if (head < 0) goto out_of_range;
  }
  if (head > ARY_MAX_SIZE - len) {
  out_of_range:
    mrb_raisef(mrb, E_INDEX_ERROR, "index %i is out of array", head);
  }
  tail = head + len;
  if (alen < len || alen < tail) {
    len = alen - head;
  }

  /* size check */