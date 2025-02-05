int AffixMgr::cpdcase_check(const char* word, int index) {
  if (utf8) {
    // <MASK>
  } else {
    unsigned char a = *(word + index - 1);
    unsigned char b = *(word + index);
    if ((csconv[a].ccase || csconv[b].ccase) && (a != '-') && (b != '-'))
      return 1;
  }
  return 0;
}