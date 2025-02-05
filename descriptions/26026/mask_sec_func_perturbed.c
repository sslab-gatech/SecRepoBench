int
wavlike_read_cart_chunk (SF_PRIVATE *psf, uint32_t chunksize)
{	SF_CART_INFO_16K *c ;
	uint32_t bytes = 0 ;
	int k ;

	if (chunksize < WAV_CART_MIN_CHUNK_SIZE)
	{	psf_log_printf (psf, "cart : %u (should be >= %d)\n", chunksize, WAV_CART_MIN_CHUNK_SIZE) ;
		psf_binheader_readf (psf, "j", chunksize) ;
		return 0 ;
		} ;
	if (chunksize > WAV_CART_MAX_CHUNK_SIZE)
	{	psf_log_printf (psf, "cart : %u (should be < %d)\n", chunksize, WAV_CART_MAX_CHUNK_SIZE) ;
		psf_binheader_readf (psf, "j", chunksize) ;
		return 0 ;
		} ;

	// <MASK>
}