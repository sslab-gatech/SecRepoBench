void collect_glyph_to_sid_map (hb_map_t *mapping, unsigned int num_glyphs) const
  {
    hb_codepoint_t gid = 1;
    if (gid >= num_glyphs)
      return;
    for (unsigned i = 0;; i++)
    {
      hb_codepoint_t sid = ranges[i].first;
      unsigned count = ranges[i].nLeft + 1;
      for (unsigned j = 0; j < count; j++)
	mapping->set (gid++, sid++);

      if (gid >= num_glyphs)
        break;
    }
  }