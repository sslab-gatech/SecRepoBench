static void
_normalize_axes_location (hb_face_t *face, hb_subset_plan_t *plan)
{
  if (plan->user_axes_location.is_empty ())
    return;

  hb_array_t<const OT::AxisRecord> axes = face->table.fvar->get_axes ();
  plan->normalized_coords.resize (axes.length);

  bool has_avar = face->table.avar->has_data ();
  const OT::SegmentMaps *seg_maps = nullptr;
  // <MASK>
  plan->all_axes_pinned = !axis_not_pinned;
}