/* Exif metadata utilities.
 * Derived from Darktable (http://www.darktable.org/)
 */

/*

    Copyright (C) 2014 Ferrero Andrea

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.


 */

/*

    These files are distributed with PhotoFlow - http://aferrero2707.github.io/PhotoFlow/

 */

#ifndef PF_EXIF_DATA_H
#define PF_EXIF_DATA_H

#include <glib.h>

#include <vips/vips.h>

#ifdef BUNDLED_EXIV2
#include <external/exiv2/include/exiv2/easyaccess.hpp>
#include <external/exiv2/include/exiv2/xmp.hpp>
#include <external/exiv2/include/exiv2/error.hpp>
#include <external/exiv2/include/exiv2/image.hpp>
#include <external/exiv2/include/exiv2/exif.hpp>
#else
//#include <exiv2/easyaccess.hpp>
//#include <exiv2/xmp.hpp>
//#include <exiv2/error.hpp>
//#include <exiv2/image.hpp>
//#include <exiv2/exif.hpp>
#include <exiv2/exiv2.hpp>
#endif


#define PF_META_EXIF_NAME "custom-exif-data"
#define PF_META_EXIV2_NAME "exiv2-data"
//#define PF_META_EXIF_NAME VIPS_META_EXIF_NAME

namespace PF
{




typedef enum {
  PF_EXIF_ORIENTATION_MIN      = 0,
  PF_EXIF_ORIENTATION_UNSPECIFIED  = 0,
  PF_EXIF_ORIENTATION_NORMAL   = 1,
  PF_EXIF_ORIENTATION_HFLIP    = 2,
  PF_EXIF_ORIENTATION_ROT_180    = 3,
  PF_EXIF_ORIENTATION_VFLIP    = 4,
  PF_EXIF_ORIENTATION_ROT_90_HFLIP = 5,
  PF_EXIF_ORIENTATION_ROT_90   = 6,
  PF_EXIF_ORIENTATION_ROT_90_VFLIP = 7,
  PF_EXIF_ORIENTATION_ROT_270    = 8,
  PF_EXIF_ORIENTATION_MAX      = 8
} ExifOrientation;


struct exif_data_t
{
  float exif_exposure;
  float exif_aperture;
  float exif_iso;
  float exif_focal_length;
  float exif_focus_distance;
  float exif_crop;
  char exif_maker[64];
  char exif_model[64];
  char exif_lens[128];
  char exif_lens_alt[128];
  char exif_datetime_taken[20];
  char camera_maker[64];
  char camera_model[64];
  char camera_alias[64];
  char camera_makermodel[128];
  char camera_legacy_makermodel[128];

  exif_data_t();
};

bool exif_read(exif_data_t* data, const char* path);

void exif_free (gpointer mem);

class exiv2_data_t
{
public:
  Exiv2::Image::AutoPtr image;
  uint8_t* blob;
  int length;
  exiv2_data_t(): image(NULL), blob(NULL), length(0) {}
};

void exiv2_free (gpointer mem);

exif_data_t* get_exif_data( VipsImage* img );

int dt_exif_write_blob(uint8_t *blob, uint32_t size, const char *path, const int sRGB, const int out_width, const int out_height);
int dt_exif_read_blob(uint8_t **buf, const char *path, const int imgid, const int out_width,
                      const int out_height, const int dng_mode);

}

#endif
