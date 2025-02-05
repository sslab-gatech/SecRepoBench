int AffixMgr::cpdcase_check(const char* word, int pos) {
  if (utf8) {
    // <MASK>
  } else {
    unsigned char a = *(word + pos - 1);
    unsigned char b = *(word + pos);
    if ((csconv[a].ccase || csconv[b].ccase) && (a != '-') && (b != '-'))
      return 1;
  }
  return 0;
}