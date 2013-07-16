/*

Copyright (C) 2013 Carnë Draug
Copyright (C) 2002-2012 Andy Adler
Copyright (C) 2008 Thomas L. Scofield
Copyright (C) 2010 David Grundberg

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, see
<http://www.gnu.org/licenses/>.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cmath>

#include "file-stat.h"
#include "oct-env.h"
#include "oct-time.h"

#include "defun-dld.h"
#include "error.h"
#include "ov-struct.h"

#include "gripes.h"

#ifdef HAVE_MAGICK

#include <Magick++.h>
#include <clocale>

octave_value_list
read_indexed_images (std::vector<Magick::Image>& imvec,
                     const Array<int>& frameidx, bool wantalpha)
{
  octave_value_list output;

  const int rows    = imvec[0].baseRows ();
  const int columns = imvec[0].baseColumns ();
  const int nframes = frameidx.length ();

  const dim_vector idim = dim_vector (rows, columns, 1, nframes);

  Array<int> idx (dim_vector (4, 1));

  Magick::ImageType type = imvec[0].type ();

  unsigned int mapsize = imvec[0].colorMapSize ();
  unsigned int i = mapsize;
  unsigned int depth = 0;
  while (i >>= 1)
    depth++;
  i = 0;
  depth--;
  while (depth >>= 1)
    i++;
  depth = 1 << i;

  switch (depth)
    {
    case 1:
    case 2:
    case 4:
    case 8:
      {
        uint8NDArray im = uint8NDArray (idim);

        idx(2) = 0;
        for (int frame = 0; frame < nframes; frame++)
          {
            imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            const Magick::IndexPacket *pix
              = imvec[frameidx(frame)].getConstIndexes ();

            i = 0;
            idx(3) = frame;

            for (int y = 0; y < rows; y++)
              {
                idx(0) = y;
                for (int x = 0; x < columns; x++)
                  {
                    idx(1) = x;
                    im(idx) = static_cast<octave_uint8> (pix[i++]);
                  }
              }
          }

        output(0) = octave_value (im);
      }
      break;

    case 16:
      {
        uint16NDArray im = uint16NDArray (idim);

        idx(2) = 0;
        for (int frame = 0; frame < nframes; frame++)
          {
            imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            const Magick::IndexPacket *pix
              = imvec[frameidx(frame)].getConstIndexes ();

            i = 0;
            idx(3) = frame;

            for (int y = 0; y < rows; y++)
              {
                idx(0) = y;
                for (int x = 0; x < columns; x++)
                  {
                    idx(1) = x;
                    im(idx) = static_cast<octave_uint16> (pix[i++]);
                  }
              }
          }

        output(0) = octave_value (im);
      }
      break;

    default:
      error ("__magic_read__: index depths greater than 16-bit are not supported");
      return octave_value_list ();
    }

  Matrix map = Matrix (mapsize, 3);
  Matrix alpha;

  switch (type)
    {
    case Magick::PaletteMatteType:
//      warning ("palettematte");
//      Matrix map (mapsize, 3);
//      Matrix alpha (mapsize, 1);
//      for (i = 0; i < mapsize; i++)
//        {
//          warning ("%d", i);
//          Magick::ColorRGB c = imvec[0].colorMap (i);
//          map(i,0) = c.red ();
//          map(i,1) = c.green ();
//          map(i,2) = c.blue ();
//          alpha(i,1) = c.alpha ();
//        }
//      break;

    case Magick::PaletteType:
      alpha = Matrix (0, 0);
      for (i = 0; i < mapsize; i++)
        {
          Magick::ColorRGB c = imvec[0].colorMap (i);
          map(i,0) = c.red ();
          map(i,1) = c.green ();
          map(i,2) = c.blue ();
        }
      break;

    default:
      error ("__magick_read__: unsupported indexed image type");
      return octave_value_list ();
    }

  if (wantalpha)
    output(2) = alpha;

  output(1) = map;

  return output;
}

template <class T>
octave_value_list
read_images (const std::vector<Magick::Image>& imvec,
             const Array<int>& frameidx, unsigned int depth)
{
  typedef typename T::element_type P;

  octave_value_list retval (3, Matrix ());

  T im;

  const int rows    = imvec[0].baseRows ();
  const int columns = imvec[0].baseColumns ();
  const int nframes = frameidx.length ();

  dim_vector idim = dim_vector (rows, columns, 1, nframes);

  Magick::ImageType type = imvec[0].type ();
  const int divisor = ((uint64_t (1) << QuantumDepth) - 1) / 
                      ((uint64_t (1) << depth) - 1);

  switch (type)
    {
    case Magick::BilevelType:           // Monochrome bi-level image
    case Magick::GrayscaleType:         // Grayscale image
      {
        im = T (idim);
        P *vec = im.fortran_vec ();

        for (int frame = 0; frame < nframes; frame++)
          {
            const Magick::PixelPacket *pix
              = imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            P *rbuf = vec;
            for (int y = 0; y < rows; y++)
              {
                for (int x = 0; x < columns; x++)
                  {
                    *rbuf = pix->red / divisor;
                    pix++;
                    rbuf += rows;
                  }
                rbuf -= rows * columns - 1;
              }

            // Next frame.
            vec += rows * columns * idim(2);
          }
        }
      break;

    case Magick::GrayscaleMatteType:    // Grayscale image with opacity
      {
        idim(2) = 2;
        im = T (idim);
        P *vec = im.fortran_vec ();

        for (int frame = 0; frame < nframes; frame++)
          {
            const Magick::PixelPacket *pix
              = imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            P *rbuf = vec;
            P *obuf = vec + rows * columns;
            for (int y = 0; y < rows; y++)
              {
                for (int x = 0; x < columns; x++)
                  {
                    *rbuf = pix->red / divisor;
                    *obuf = pix->opacity / divisor;
                    pix++;
                    rbuf += rows;
                    obuf += rows;
                  }
                rbuf -= rows * columns - 1;
                obuf -= rows * columns - 1;
              }

            // Next frame.
            vec += rows * columns * idim(2);
          }
        }
      break;

    case Magick::PaletteType:           // Indexed color (palette) image
    case Magick::TrueColorType:         // Truecolor image
      {
        idim(2) = 3;
        im = T (idim);
        P *vec = im.fortran_vec ();

        for (int frame = 0; frame < nframes; frame++)
          {
            const Magick::PixelPacket *pix
              = imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            P *rbuf = vec;
            P *gbuf = vec + rows * columns;
            P *bbuf = vec + rows * columns * 2;
            for (int y = 0; y < rows; y++)
              {
                for (int x = 0; x < columns; x++)
                  {
                    *rbuf = pix->red / divisor;
                    *gbuf = pix->green / divisor;
                    *bbuf = pix->blue / divisor;
                    pix++;
                    rbuf += rows;
                    gbuf += rows;
                    bbuf += rows;
                  }
                rbuf -= rows * columns - 1;
                gbuf -= rows * columns - 1;
                bbuf -= rows * columns - 1;
              }

            // Next frame.
            vec += rows * columns * idim(2);
          }
        }
      break;

    case Magick::PaletteMatteType:      // Indexed color (palette) image with opacity
    case Magick::TrueColorMatteType:    // Truecolor image with opacity
    case Magick::ColorSeparationType:   // Cyan/Yellow/Magenta/Black (CYMK) image
      {
        idim(2) = 4;
        im = T (idim);
        P *vec = im.fortran_vec ();

        for (int frame = 0; frame < nframes; frame++)
          {
            const Magick::PixelPacket *pix
              = imvec[frameidx(frame)].getConstPixels (0, 0, columns, rows);

            P *rbuf = vec;
            P *gbuf = vec + rows * columns;
            P *bbuf = vec + rows * columns * 2;
            P *obuf = vec + rows * columns * 3;
            for (int y = 0; y < rows; y++)
              {
                for (int x = 0; x < columns; x++)
                  {
                    *rbuf = pix->red / divisor;
                    *gbuf = pix->green / divisor;
                    *bbuf = pix->blue / divisor;
                    *obuf = pix->opacity / divisor;
                    pix++;
                    rbuf += rows;
                    gbuf += rows;
                    bbuf += rows;
                    obuf += rows;
                  }
                rbuf -= rows * columns - 1;
                gbuf -= rows * columns - 1;
                bbuf -= rows * columns - 1;
                obuf -= rows * columns - 1;
              }

            // Next frame.
            vec += rows * columns * idim(2);
          }
        }
      break;

    default:
      error ("__magick_read__: undefined Magick++ image type");
      return retval;
    }

  retval(0) = im;

  return retval;
}

void static
read_file (const std::string filename, std::vector<Magick::Image>& imvec)
{
  try
    {
      // Read a file into vector of image objects
      Magick::readImages (&imvec, filename);
    }
  catch (Magick::Warning& w)
    {
      warning ("Magick++ warning: %s", w.what ());
    }
  catch (Magick::ErrorCoder& e)
    {
      // FIXME: there's a WarningCoder and ErrorCoder. Shouldn't this
      // exception cause an error?
      warning ("Magick++ coder error: %s", e.what ());
    }
  catch (Magick::Exception& e)
    {
      error ("Magick++ exception: %s", e.what ());
      error_state = 1;
    }
}


static void
maybe_initialize_magick (void)
{
  static bool initialized = false;

  if (! initialized)
    {
      // Save locale as GraphicsMagick might change this (fixed in
      // GraphicsMagick since version 1.3.13 released on December 24, 2011)
      const char *static_locale = setlocale (LC_ALL, NULL);
      const std::string locale (static_locale);

      const std::string program_name = octave_env::get_program_invocation_name ();

      Magick::InitializeMagick (program_name.c_str ());

      // Restore locale from before GraphicsMagick initialisation
      setlocale (LC_ALL, locale.c_str ());

      if (QuantumDepth < 32)
        warning ("your version of %s limits images to %d bits per pixel",
                 MagickPackageName, QuantumDepth);

      initialized = true;
    }
}
#endif

DEFUN_DLD (__magick_read__, args, nargout,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {[@var{img}, @var{map}, @var{alpha}] =} __magick_read__ (@var{fname}, @var{options})\n\
Read image with GraphicsMagick or ImageMagick.\n\
\n\
This is a private internal function not intended for direct use.  Instead\n\
use @code{imread}.\n\
\n\
@seealso{imfinfo, imformats, imread, imwrite}\n\
@end deftypefn")
{
  octave_value_list output;

#ifndef HAVE_MAGICK
  gripe_disabled_feature ("imread", "Image IO");
#else

  maybe_initialize_magick ();

  if (args.length () != 2 || ! args(0).is_string ())
    {
      print_usage ();
      return output;
    }

  const octave_map options = args(1).map_value ();
  if (error_state)
    {
      error ("__magick_read__: OPTIONS must be a struct");
    }

  std::vector<Magick::Image> imvec;
  read_file (args(0).string_value (), imvec);
  if (error_state)
    {
      return output;
    }

  const int nframes = imvec.size ();
  Array<int> frameidx;

  const octave_value indexes = options.getfield ("index")(0);
  if (indexes.is_string () && indexes.string_value () == "all")
    {
      frameidx = Array<int> (dim_vector (1, nframes));
      for (int i = 0; i < nframes; i++)
        {
          frameidx(i) = i;
        }
    }
  else
    {
      frameidx = indexes.int_vector_value ();
      if (error_state)
        {
          error ("__magick_read__: invalid value for Index/Frame");
        }
      // Fix indexex from base 1 to base 0, and at the same time, make
      // sure none of the indexes is outside the range of image number.
      const int n = frameidx.nelem ();
      for (int i = 0; i < n; i++)
        {
          frameidx(i)--;
          if (frameidx(i) < 0 || frameidx(i) > nframes - 1)
            {
              error ("imread: index/frames specified are outside the number of images");
              return output;
            }
        }
    }

  const Magick::ClassType klass = imvec[0].classType ();

  // PseudoClass:
  // Image is composed of pixels which specify an index in a color palette.
  if (klass == Magick::PseudoClass && nargout > 1)
    {
      output = read_indexed_images (imvec, frameidx, (nargout == 3));
    }
  // If not PseudoClass then it must be DirectClass: Image is composed of
  // pixels which represent literal color values.
  else
    {
      unsigned int depth = imvec[0].modulusDepth ();
      if (depth > 1)
        {
          --depth;
          int i = 1;
          while (depth >>= 1)
            i++;
          depth = 1 << i;
        }

      switch (depth)
        {
        case 1:
          output = read_images<boolNDArray> (imvec, frameidx, depth);
          break;

        case 2:
        case 4:
        case 8:
          output = read_images<uint8NDArray> (imvec, frameidx, depth) ;
          break;

        case 16:
          output = read_images<uint16NDArray> (imvec, frameidx, depth);
          break;

        case 32:
        case 64:
        default:
          error ("__magick_read__: image depths greater than 16-bit are not supported");
        }
    }

#endif
  return output;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/

#ifdef HAVE_MAGICK

static void
encode_bool_image (std::vector<Magick::Image>& imvec, const octave_value& img)
{
  unsigned int nframes = 1;
  boolNDArray m = img.bool_array_value ();

  dim_vector dsizes = m.dims ();
  if (dsizes.length () == 4)
    nframes = dsizes(3);

  Array<octave_idx_type> idx (dim_vector (dsizes.length (), 1));

  octave_idx_type rows = m.rows ();
  octave_idx_type columns = m.columns ();

  for (unsigned int ii = 0; ii < nframes; ii++)
    {
      Magick::Image im (Magick::Geometry (columns, rows), "black");
      im.classType (Magick::DirectClass);
      im.depth (1);

      for (int y = 0; y < columns; y++)
        {
          idx(1) = y;

          for (int x = 0; x < rows; x++)
            {
              if (nframes > 1)
                {
                  idx(2) = 0;
                  idx(3) = ii;
                }

              idx(0) = x;

              if (m(idx))
                im.pixelColor (y, x, "white");
            }
        }

      im.quantizeColorSpace (Magick::GRAYColorspace);
      im.quantizeColors (2);
      im.quantize ();

      imvec.push_back (im);
    }
}

template <class T>
static void
encode_uint_image (std::vector<Magick::Image>& imvec,
                   const octave_value& img,
                   const bool has_map)
{
  unsigned int bitdepth = 0;
  T m;

  if (img.is_uint8_type ())
    {
      bitdepth = 8;
      m = img.uint8_array_value ();
    }
  else if (img.is_uint16_type ())
    {
      bitdepth = 16;
      m = img.uint16_array_value ();
    }
  else
    error ("__magick_write__: invalid image class");

  const dim_vector dsizes = m.dims ();
  unsigned int nframes = 1;
  if (dsizes.length () == 4)
    nframes = dsizes(3);

  const bool is_color = ((dsizes.length () > 2) && (dsizes(2) > 2));
  const bool has_alpha = (dsizes.length () > 2 && (dsizes(2) == 2 || dsizes(2) == 4));

  Array<octave_idx_type> idx (dim_vector (dsizes.length (), 1));
  octave_idx_type rows = m.rows ();
  octave_idx_type columns = m.columns ();

  unsigned int div_factor = (1 << bitdepth) - 1;

  for (unsigned int ii = 0; ii < nframes; ii++)
    {
      Magick::Image im (Magick::Geometry (columns, rows), "black");

      im.depth (bitdepth);

      if (has_map)
        im.classType (Magick::PseudoClass);
      else
        im.classType (Magick::DirectClass);

      if (is_color)
        {
          if (has_alpha)
            im.type (Magick::TrueColorMatteType);
          else
            im.type (Magick::TrueColorType);

          Magick::ColorRGB c;

          for (int y = 0; y < columns; y++)
            {
              idx(1) = y;

              for (int x = 0; x < rows; x++)
                {
                  idx(0) = x;

                  if (nframes > 1)
                    idx(3) = ii;

                  idx(2) = 0;
                  c.red (static_cast<double>(m(idx)) / div_factor);

                  idx(2) = 1;
                  c.green (static_cast<double>(m(idx)) / div_factor);

                  idx(2) = 2;
                  c.blue (static_cast<double>(m(idx)) / div_factor);

                  if (has_alpha)
                    {
                      idx(2) = 3;
                      c.alpha (static_cast<double>(m(idx)) / div_factor);
                    }

                  im.pixelColor (y, x, c);
                }
            }
        }
      else
        {
          if (has_alpha)
            im.type (Magick::GrayscaleMatteType);
          else
            im.type (Magick::GrayscaleType);

          Magick::ColorGray c;

          for (int y = 0; y < columns; y++)
            {
              idx(1) = y;

              for (int x=0; x < rows; x++)
                {
                  idx(0) = x;

                  if (nframes > 1)
                    {
                      idx(2) = 0;
                      idx(3) = ii;
                    }

                  if (has_alpha)
                    {
                      idx(2) = 1;
                      c.alpha (static_cast<double>(m(idx)) / div_factor);
                      idx(2) = 0;
                    }

                  c.shade (static_cast<double>(m(idx)) / div_factor);

                  im.pixelColor (y, x, c);
                }
            }

          im.quantizeColorSpace (Magick::GRAYColorspace);
          im.quantizeColors (1 << bitdepth);
          im.quantize ();
        }

      imvec.push_back (im);
    }
}

// FIXME: this will be needed to write indexed images
//static void
//encode_map (std::vector<Magick::Image>& imvec, const NDArray& cmap)
//{
//  unsigned int mapsize = cmap.dim1 ();

//  for (size_t fnum = 0; fnum < imvec.size (); fnum++)
//    {
//      imvec[fnum].colorMapSize (mapsize);
//      imvec[fnum].type (Magick::PaletteType);
//    }

//  for (unsigned int ii = 0; ii < mapsize; ii++)
//    {
//      Magick::ColorRGB c (cmap(ii,0), cmap(ii,1), cmap(ii,2));

//      // FIXME -- is this case needed?
//      if (cmap.dim2 () == 4)
//        c.alpha (cmap(ii,3));

//      try
//        {
//          for_each (imvec.begin (), imvec.end (),
//                    Magick::colorMapImage (ii, c));
//        }
//      catch (Magick::Warning& w)
//        {
//          warning ("Magick++ warning: %s", w.what ());
//        }
//      catch (Magick::ErrorCoder& e)
//        {
//          warning ("Magick++ coder error: %s", e.what ());
//        }
//      catch (Magick::Exception& e)
//        {
//          error ("Magick++ exception: %s", e.what ());
//        }
//    }
//}

void static
write_file (const std::string filename,
            const std::string ext,
            std::vector<Magick::Image>& imvec)
{
  try
    {
      Magick::writeImages (imvec.begin (), imvec.end (), ext + ":" + filename);
    }
  catch (Magick::Warning& w)
    {
      warning ("Magick++ warning: %s", w.what ());
    }
  catch (Magick::ErrorCoder& e)
    {
      warning ("Magick++ coder error: %s", e.what ());
    }
  catch (Magick::Exception& e)
    {
      error ("Magick++ exception: %s", e.what ());
      error_state = 1;
    }
}

#endif

DEFUN_DLD (__magick_write__, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} __magick_write__ (@var{fname}, @var{fmt}, @var{img}, @var{map}, @var{options})\n\
Write image with GraphicsMagick or ImageMagick.\n\
\n\
This is a private internal function not intended for direct use.  Instead\n\
use @code{imwrite}.\n\
\n\
@seealso{imfinfo, imformats, imread, imwrite}\n\
@end deftypefn")
{
  octave_value_list retval;

#ifndef HAVE_MAGICK
  gripe_disabled_feature ("imwrite", "Image IO");
#else

  maybe_initialize_magick ();

  if (args.length () != 5 || ! args(0).is_string () || ! args(1).is_string ())
    {
      print_usage ();
      return retval;
    }
  const std::string filename = args(0).string_value ();
  const std::string ext      = args(1).string_value ();

  const octave_map options   = args(4).map_value ();
  if (error_state)
    {
      error ("__magick_write__: OPTIONS must be a struct");
    }

  const octave_value img  = args(2);
  const Matrix       cmap = args(3).matrix_value ();
  if (error_state)
    {
      error ("__magick_write__: invalid IMG or MAP");
    }
  const bool is_indexed = ! cmap.is_empty ();

  // Create vector with the images to write
  std::vector<Magick::Image> imvec;
  if (img.is_bool_type ())
    {
      encode_bool_image (imvec, img);
    }
  else if (img.is_uint8_type ())
    {
      encode_uint_image<uint8NDArray> (imvec, img, is_indexed);
    }
  else if (img.is_uint16_type ())
    {
      encode_uint_image<uint16NDArray> (imvec, img, is_indexed);
    }
  else
    {
      error ("__magick_write__: image type not supported");
      return retval;
    }
  const int nframes = imvec.size ();

  // Add colormap to image
  if (is_indexed)
    {
    // FIXME: this should be implemented. At the moment, imwrite is doing the
    //        conversion in case of indexed images.
      error ("__magick_write__: direct saving of indexed images not currently supported; use ind2rgb and save converted image");
//      encode_map (imvec, cmap);
      return retval;
    }

  // Set quality.
  // FIXME What happens when we try to set with formats that do not support it?
  const unsigned int quality = options.getfield ("quality")(0).int_value ();
  for (int i = 0; i < nframes; i++)
    {
      imvec[i].quality (quality);
    }

  // Finally, save the file.
  // If writemode is set to append, read the image first, append to it,
  // and then save it. But even if set to append, make sure anything was
  // read at all.
  const std::string writemode = options.getfield ("writemode")(0).string_value ();
  std::vector<Magick::Image> ini_imvec;
  if (writemode == "append")
    {
      read_file (filename, ini_imvec);
      if (error_state)
        {
          return retval;
        }
    }

  if (ini_imvec.size () > 0)
    {
      ini_imvec.insert (ini_imvec.end (), imvec.begin (), imvec.end ());
      write_file (filename, ext, ini_imvec);
      if (error_state)
        {
          return retval;
        }
    }
  else
    {
      write_file (filename, ext, imvec);
      if (error_state)
        {
          return retval;
        }
    }

#endif
  return retval;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/

#ifdef HAVE_MAGICK

template<class T>
static octave_value
magick_to_octave_value (const T magick)
{
  return octave_value (magick);
}

static octave_value
magick_to_octave_value (const Magick::EndianType magick)
{
  switch (magick)
    {
      case Magick::LSBEndian:
        return octave_value ("little-endian");

      case Magick::MSBEndian:
        return octave_value ("big-endian");

      default:
        return octave_value ("undefined");
    }
}

static octave_value
magick_to_octave_value (const Magick::ResolutionType magick)
{
  switch (magick)
    {
      case Magick::PixelsPerInchResolution:
        return octave_value ("pixels per inch");

      case Magick::PixelsPerCentimeterResolution:
        return octave_value ("pixels per centimeter");

      default:
        return octave_value ("undefined");
    }
}

static octave_value
magick_to_octave_value (const Magick::ImageType magick)
{
  switch (magick)
    {
      case Magick::BilevelType:
      case Magick::GrayscaleType:
      case Magick::GrayscaleMatteType:
        return octave_value ("grayscale");

      case Magick::PaletteType:
      case Magick::PaletteMatteType:
        return octave_value ("indexed");

      case Magick::TrueColorType:
      case Magick::TrueColorMatteType:
      case Magick::ColorSeparationType:
        return octave_value ("truecolor");

      default:
        return octave_value ("undefined");
    }
}

// We put this in a try-block because GraphicsMagick will throw
// exceptions if a parameter isn't present in the current image.
#define GET_PARAM(NAME, OUTNAME) \
  try \
    { \
      info.contents (OUTNAME)(frame,0) = magick_to_octave_value (im.NAME ()); \
    } \
  catch (Magick::Warning& w) \
    { \
    }

#endif

DEFUN_DLD (__magick_finfo__, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} __magick_finfo__ (@var{fname})\n\
Read image information with GraphicsMagick or ImageMagick.\n\
\n\
This is a private internal function not intended for direct use.  Instead\n\
use @code{imfinfo}.\n\
\n\
@seealso{imfinfo, imformats, imread, imwrite}\n\
@end deftypefn")
{
  octave_value retval;

#ifndef HAVE_MAGICK
  gripe_disabled_feature ("imfinfo", "Image IO");
#else

  maybe_initialize_magick ();

  if (args.length () < 1 || ! args (0).is_string ())
    {
      print_usage ();
      return retval;
    }

  const std::string filename = args (0).string_value ();

  try
    {
      // Read the file.
      std::vector<Magick::Image> imvec;
      Magick::readImages (&imvec, args(0).string_value ());
      int nframes = imvec.size ();

      // Create the right size for the output.

      static const char *fields[] =
        {
          "Filename",
          "FileModDate",
          "FileSize",
          "Height",
          "Width",
          "BitDepth",
          "Format",
          "LongFormat",
          "XResolution",
          "YResolution",
          "TotalColors",
          "TileName",
          "AnimationDelay",
          "AnimationIterations",
          "ByteOrder",
          "Gamma",
          "Matte",
          "ModulusDepth",
          "Quality",
          "QuantizeColors",
          "ResolutionUnits",
          "ColorType",
          "View",
          0
        };

      octave_map info (dim_vector (nframes, 1), string_vector (fields));

      file_stat fs (filename);

      std::string filetime;

      if (fs)
        {
          octave_localtime mtime = fs.mtime ();

          filetime = mtime.strftime ("%e-%b-%Y %H:%M:%S");
        }
      else
        {
          std::string msg = fs.error ();

          error ("imfinfo: error reading '%s': %s",
                 filename.c_str (), msg.c_str ());

          return retval;
        }

      // For each frame in the image (some images contain multiple
      // layers, each to be treated like a separate image).
      for (int frame = 0; frame < nframes; frame++)
        {
          Magick::Image im = imvec[frame];

          // Add file name and timestamp.
          info.contents ("Filename")(frame,0) = filename;
          info.contents ("FileModDate")(frame,0) = filetime;

          // Annoying CamelCase naming is for Matlab compatibility.
          GET_PARAM (fileSize, "FileSize")
          GET_PARAM (rows, "Height")
          GET_PARAM (columns, "Width")
          GET_PARAM (depth, "BitDepth")
          GET_PARAM (magick, "Format")
          GET_PARAM (format, "LongFormat")
          GET_PARAM (xResolution, "XResolution")
          GET_PARAM (yResolution, "YResolution")
          GET_PARAM (totalColors, "TotalColors")
          GET_PARAM (tileName, "TileName")
          GET_PARAM (animationDelay, "AnimationDelay")
          GET_PARAM (animationIterations, "AnimationIterations")
          GET_PARAM (endian, "ByteOrder")
          GET_PARAM (gamma, "Gamma")
          GET_PARAM (matte, "Matte")
          GET_PARAM (modulusDepth, "ModulusDepth")
          GET_PARAM (quality, "Quality")
          GET_PARAM (quantizeColors, "QuantizeColors")
          GET_PARAM (resolutionUnits, "ResolutionUnits")
          GET_PARAM (type, "ColorType")
          GET_PARAM (view, "View")
        }

      retval = octave_value (info);
    }
  catch (Magick::Warning& w)
    {
      warning ("Magick++ warning: %s", w.what ());
    }
  catch (Magick::ErrorCoder& e)
    {
      warning ("Magick++ coder error: %s", e.what ());
    }
  catch (Magick::Exception& e)
    {
      error ("Magick++ exception: %s", e.what ());
      return retval;
    }
#endif
  return retval;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/

#undef GET_PARAM

DEFUN_DLD (__magick_formats__, args, ,
  "-*- texinfo -*-\n\
@deftypefn {Loadable Function} {} __magick_imformats__ (@var{formats})\n\
Fill formats info with GraphicsMagick CoderInfo.\n\
\n\
@seealso{imfinfo, imformats, imread, imwrite}\n\
@end deftypefn")
{
  octave_value retval;
#ifndef HAVE_MAGICK
  gripe_disabled_feature ("imformats", "Image IO");
#else
  if (args.length () != 1 || ! args (0).is_map ())
    {
      print_usage ();
      return retval;
    }
  octave_map formats = args(0).map_value ();

  maybe_initialize_magick ();
  for (octave_idx_type idx = 0; idx < formats.numel (); idx++)
    {
      try
        {
          octave_scalar_map fmt = formats.checkelem (idx);
          Magick::CoderInfo coder (fmt.getfield ("coder").string_value ());

          fmt.setfield ("description", octave_value (coder.description ()));
          fmt.setfield ("multipage", coder.isMultiFrame () ? true : false);
          // default for read and write is a function handle. If we can't
          // read or write them, them set it to an empty value
          if (! coder.isReadable ())
            fmt.setfield ("read",  Matrix ());
          if (! coder.isWritable ())
            fmt.setfield ("write", Matrix ());
          formats.fast_elem_insert (idx, fmt);
        }
      catch (Magick::Exception& e)
        {
          // Exception here are missing formats. So we remove the format
          // from the structure and reduce idx.
          formats.delete_elements (idx);
          idx--;
        }
    }
  retval = formats;
#endif
  return retval;
}

/*
## No test needed for internal helper function.
%!assert (1)
*/
