static int
zipx_lzma_alone_init(struct archive_read *a, struct zip *zip)
{
	lzma_ret r;
	const uint8_t* p;

#pragma pack(push)
#pragma pack(1)
	struct _alone_header {
	    uint8_t bytes[5];
	    uint64_t uncompressed_size;
	} alone_header;
#pragma pack(pop)

	if(zip->zipx_lzma_valid) {
		lzma_end(&zip->zipx_lzma_stream);
		zip->zipx_lzma_valid = 0;
	}

	/* To unpack ZIPX's "LZMA" (id 14) stream we can use standard liblzma
	 * that is a part of XZ Utils. The stream format stored inside ZIPX
	 * file is a modified "lzma alone" file format, that was used by the
	 * `lzma` utility which was later deprecated in favour of `xz` utility.
 	 * Since those formats are nearly the same, we can use a standard
	 * "lzma alone" decoder from XZ Utils. */

	memset(&zip->zipx_lzma_stream, 0, sizeof(zip->zipx_lzma_stream));
	r = lzma_alone_decoder(&zip->zipx_lzma_stream, UINT64_MAX);
	if (r != LZMA_OK) {
		archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
		    "lzma initialization failed(%d)", r);

		return (ARCHIVE_FAILED);
	}

	/* Flag the cleanup function that we want our lzma-related structures
	 * to be freed later. */
	zip->zipx_lzma_valid = 1;

	// <MASK>

	if(p[2] != 0x05 || p[3] != 0x00) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Invalid lzma data");
		return (ARCHIVE_FATAL);
	}

	/* Prepare an lzma alone header: copy the lzma_params blob into
	 * a proper place into the lzma alone header. */
	memcpy(&alone_header.bytes[0], p + 4, 5);

	/* Initialize the 'uncompressed size' field to unknown; we'll manually
	 * monitor how many bytes there are still to be uncompressed. */
	alone_header.uncompressed_size = UINT64_MAX;

	if(!zip->uncompressed_buffer) {
		zip->uncompressed_buffer_size = 256 * 1024;
		zip->uncompressed_buffer =
			(uint8_t*) malloc(zip->uncompressed_buffer_size);

		if (zip->uncompressed_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "No memory for lzma decompression");
			return (ARCHIVE_FATAL);
		}
	}

	zip->zipx_lzma_stream.next_in = (void*) &alone_header;
	zip->zipx_lzma_stream.avail_in = sizeof(alone_header);
	zip->zipx_lzma_stream.total_in = 0;
	zip->zipx_lzma_stream.next_out = zip->uncompressed_buffer;
	zip->zipx_lzma_stream.avail_out = zip->uncompressed_buffer_size;
	zip->zipx_lzma_stream.total_out = 0;

	/* Feed only the header into the lzma alone decoder. This will
	 * effectively initialize the decoder, and will not produce any
	 * output bytes yet. */
	r = lzma_code(&zip->zipx_lzma_stream, LZMA_RUN);
	if (r != LZMA_OK) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
		    "lzma stream initialization error");
		return ARCHIVE_FATAL;
	}

	/* We've already consumed some bytes, so take this into account. */
	__archive_read_consume(a, 9);
	zip->entry_bytes_remaining -= 9;
	zip->entry_compressed_bytes_read += 9;

	zip->decompress_init = 1;
	return (ARCHIVE_OK);
}