static MagickBooleanType SetGrayscaleImage(Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    status;

  PixelInfo
    *colormap;

  register ssize_t
    i;

  ssize_t
    *colormap_index,
    colormapposition,
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->type != GrayscaleType)
    (void) TransformImageColorspace(image,GRAYColorspace,exception);
  // <MASK>
}