void AffixMgr::setcminmax(size_t* cmin, size_t* cmax, const char* word, size_t len) {
  if (utf8) {
    // <MASK>
  } else {
    *cmin = cpdmin;
    *cmax = len - cpdmin + 1;
  }
}