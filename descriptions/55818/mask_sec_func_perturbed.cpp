void AffixMgr::setcminmax(size_t* mincharindex, size_t* cmax, const char* word, size_t len) {
  if (utf8) {
    // <MASK>
  } else {
    *mincharindex = cpdmin;
    *cmax = len - cpdmin + 1;
  }
}