bool serialize (hb_serialize_context_t *c,
                  Iterator iterator)
  {
    TRACE_SERIALIZE (this);
    if (unlikely (!c->extend_min (*this))) return_trace (false);

    if (unlikely (!iterator))
    {
      startGlyph = 0;
      classValue.len = 0;
      return_trace (true);
    }

    hb_codepoint_t glyph_min = (*iterator).first;
    hb_codepoint_t glyph_max = + iterator
                               | hb_map (hb_first)
                               | hb_reduce (hb_max, 0u);
    // <MASK>
    return_trace (true);
  }