static hb_set_t *
_populate_gids_to_retain (hb_face_t *fontface,
			  const hb_set_t *unicodes,
			  bool close_over_gsub,
			  hb_set_t *unicodes_to_retain,
			  hb_map_t *codepoint_to_glyph,
			  hb_vector_t<hb_codepoint_t> *glyphs)
{
  OT::cmap::accelerator_t cmap;
  OT::glyf::accelerator_t glyf;
  cmap.init (fontface);
  glyf.init (fontface);

  hb_set_t *initial_gids_to_retain = hb_set_create ();
  initial_gids_to_retain->add (0); // Not-def

  hb_codepoint_t cp = HB_SET_VALUE_INVALID;
  while (unicodes->next (&cp))
  {
    hb_codepoint_t gid;
    if (!cmap.get_nominal_glyph (cp, &gid))
    {
      DEBUG_MSG(SUBSET, nullptr, "Drop U+%04X; no gid", cp);
      continue;
    }
    unicodes_to_retain->add (cp);
    codepoint_to_glyph->set (cp, gid);
    initial_gids_to_retain->add (gid);
  }

  if (close_over_gsub)
    // Add all glyphs needed for GSUB substitutions.
    _gsub_closure (fontface, initial_gids_to_retain);

  // Populate a full set of glyphs to retain by adding all referenced
  // composite glyphs.
  hb_codepoint_t gid = HB_SET_VALUE_INVALID;
  hb_set_t *all_gids_to_retain = hb_set_create ();
  while (initial_gids_to_retain->next (&gid))
  {
    _add_gid_and_children (glyf, gid, all_gids_to_retain);
  }
  hb_set_destroy (initial_gids_to_retain);

  _remove_invalid_gids (all_gids_to_retain, fontface->get_num_glyphs ());

  glyphs->alloc (all_gids_to_retain->get_population ());
  gid = HB_SET_VALUE_INVALID;
  while (all_gids_to_retain->next (&gid))
    glyphs->push (gid);

  glyf.fini ();
  cmap.fini ();

  return all_gids_to_retain;
}