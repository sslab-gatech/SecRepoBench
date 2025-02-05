bool AffixMgr::parse_phonetable(const std::string& line, FileMgr* af) {
  if (phone) {
    HUNSPELL_WARNING(stderr, "error: line %d: multiple table definitions\n",
                     af->getlinenum());
    return false;
  }
  // <MASK>
  return true;
}