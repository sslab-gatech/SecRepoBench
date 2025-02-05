bool get_contour_points (contour_point_vector_t &points_ /* OUT */,
			   bool phantom_only = false) const
  {
    const HBUINT16 *endPtsOfContours = &StructAfter<HBUINT16> (header);
    int num_contours = header.numberOfContours;
    assert (num_contours > 0);
    /* One extra item at the end, for the instruction-count below. */
    if (unlikely (!bytes.check_range (&endPtsOfContours[num_contours]))) return false;
    unsigned int num_points = endPtsOfContours[num_contours - 1] + 1;

    points_.alloc (num_points + 4, true); // Allocate for phantom points, to avoid a possible copy
    if (!points_.resize (num_points)) return false;
    if (phantom_only) return true;

    for (int i = 0; i < num_contours; i++)
      points_[endPtsOfContours[i]].is_end_point = true;

    /* Skip instructions */
    const HBUINT8 *p = &StructAtOffset<HBUINT8> (&endPtsOfContours[num_contours + 1],
						 endPtsOfContours[num_contours]);

    if (unlikely ((const char *) p < bytes.arrayZ)) return false; /* Unlikely overflow */
    const HBUINT8 *end = (const HBUINT8 *) (bytes.arrayZ + bytes.length);
    if (unlikely (p >= end)) return false;

    /* Read x & y coordinates */
    return read_flags (p, points_, end)
        && read_points (p, points_, end, &contour_point_t::x,
			FLAG_X_SHORT, FLAG_X_SAME)
	&& read_points (p, points_, end, &contour_point_t::y,
			FLAG_Y_SHORT, FLAG_Y_SAME);
  }