bool is_lookup_done (unsigned int lookup_index)
  {
    if (done_lookups_glyph_count->in_error () ||
        done_lookups_glyph_set->in_error ())
      return true;

    /* Have we visited this lookup with the current set of glyphs? */
    if (done_lookups_glyph_count->get (lookup_index) != glyphs->get_population ())
    {
      done_lookups_glyph_count->set (lookup_index, glyphs->get_population ());

      if (!done_lookups_glyph_set->get (lookup_index))
      // <MASK>
    }

    hb_set_t *covered_glyph_set = done_lookups_glyph_set->get (lookup_index);
    if (parent_active_glyphs ()->is_subset (covered_glyph_set))
      return true;

    hb_set_union (covered_glyph_set, parent_active_glyphs ());
    return false;
  }