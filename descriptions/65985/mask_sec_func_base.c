GF_Err mpgviddmx_process(GF_Filter *filter)
{
	GF_MPGVidDmxCtx *ctx = gf_filter_get_udta(filter);
	GF_FilterPacket *pck, *dst_pck;
	u64 byte_offset;
	s64 vosh_start = -1;
	s64 vosh_end = -1;
	GF_Err e;
	char *data;
	u8 *start;
	u32 pck_size;
	s32 remain;

	//always reparse duration
	if (!ctx->duration.num)
		mpgviddmx_check_dur(filter, ctx);

	pck = gf_filter_pid_get_packet(ctx->ipid);
	if (!pck) {
		if (gf_filter_pid_is_eos(ctx->ipid)) {
			mpgviddmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE, GF_TRUE);
			if (ctx->opid)
				gf_filter_pid_set_eos(ctx->opid);
			if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
			ctx->src_pck = NULL;
			return GF_EOS;
		}
		return GF_OK;
	}

	data = (char *) gf_filter_pck_get_data(pck, &pck_size);
	byte_offset = gf_filter_pck_get_byte_offset(pck);

	start = data;
	remain = pck_size;

	//input pid was muxed - we flushed pending data , update cts
	if (!ctx->resume_from && ctx->timescale) {
		if (!ctx->notime) {
			u64 ts = gf_filter_pck_get_cts(pck);
			if (ts != GF_FILTER_NO_TS) {
				if (!ctx->cts || !ctx->recompute_cts)
					ctx->cts = ts;
			}
			ts = gf_filter_pck_get_dts(pck);
			if (ts != GF_FILTER_NO_TS) {
				if (!ctx->dts || !ctx->recompute_cts) {
					ctx->dts = ts;
				}

				if (!ctx->prev_dts) ctx->prev_dts = ts;
				else if (ctx->prev_dts != ts) {
					u64 diff = ts;
					diff -= ctx->prev_dts;
					if (!ctx->cur_fps.den)
						ctx->cur_fps.den = (u32) diff;
					else if (ctx->cur_fps.den > diff)
						ctx->cur_fps.den = (u32) diff;

					ctx->prev_dts = ts;
				}
			}
		}

		gf_filter_pck_get_framing(pck, &ctx->input_is_au_start, &ctx->input_is_au_end);
		//we force it to true to deal with broken avi packaging where a video AU is split across several AVI video frames
		ctx->input_is_au_end = GF_FALSE;
		//this will force CTS recomput of each frame
		if (ctx->recompute_cts) ctx->input_is_au_start = GF_FALSE;
		if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
		ctx->src_pck = pck;
		gf_filter_pck_ref_props(&ctx->src_pck);
	}

	//we stored some data to find the complete vosh, aggregate this packet with current one
	if (!ctx->resume_from && ctx->hdr_store_size) {
		if (ctx->hdr_store_alloc < ctx->hdr_store_size + pck_size) {
			ctx->hdr_store_alloc = ctx->hdr_store_size + pck_size;
			ctx->hdr_store = gf_realloc(ctx->hdr_store, sizeof(char)*ctx->hdr_store_alloc);
		}
		memcpy(ctx->hdr_store + ctx->hdr_store_size, data, sizeof(char)*pck_size);
		if (byte_offset != GF_FILTER_NO_BO) {
			if (byte_offset >= ctx->hdr_store_size)
				byte_offset -= ctx->hdr_store_size;
			else
				byte_offset = GF_FILTER_NO_BO;
		}
		ctx->hdr_store_size += pck_size;
		start = data = ctx->hdr_store;
		remain = pck_size = ctx->hdr_store_size;
	}

	if (ctx->resume_from) {
		if (gf_filter_pid_would_block(ctx->opid))
			return GF_OK;

		//resume from data copied internally
		if (ctx->hdr_store_size) {
			gf_assert(ctx->resume_from <= ctx->hdr_store_size);
			start = data = ctx->hdr_store + ctx->resume_from;
			remain = pck_size = ctx->hdr_store_size - ctx->resume_from;
		} else {
			gf_assert(remain >= (s32) ctx->resume_from);
			start += ctx->resume_from;
			remain -= ctx->resume_from;
		}
		ctx->resume_from = 0;
	}

	if (!ctx->bs) {
		ctx->bs = gf_bs_new(start, remain, GF_BITSTREAM_READ);
	} else {
		gf_bs_reassign_buffer(ctx->bs, start, remain);
	}
	if (!ctx->vparser) {
		ctx->vparser = gf_m4v_parser_bs_new(ctx->bs, ctx->is_mpg12);
	}


	while (remain) {
		Bool full_frame;
		u8 *pck_data;
		s32 current;
		u8 sc_type, forced_sc_type=0;
		Bool sc_type_forced = GF_FALSE;
		Bool skip_pck = GF_FALSE;
		u8 ftype;
		u32 tinc;
		u64 size=0;
		u64 fstart=0;
		Bool is_coded;
		u32 bytes_from_store = 0;
		u32 hdr_offset = 0;
		Bool copy_last_bytes = GF_FALSE;

		//not enough bytes to parse start code
		if (remain<5) {
			memcpy(ctx->hdr_store, start, remain);
			ctx->bytes_in_header = remain;
			break;
		}
		current = -1;

		//we have some potential bytes of a start code in the store, copy some more bytes and check if valid start code.
		// <MASK>
	}
	gf_filter_pid_drop_packet(ctx->ipid);

	return GF_OK;
}