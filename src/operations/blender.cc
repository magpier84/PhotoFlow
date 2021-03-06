/* 
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

#include "blender.hh"
#include "../base/image_hierarchy.hh"
#include "../base/processor.hh"
#include "../base/new_operation.hh"


bool PF::BlenderPar::adjust_geom( VipsImage* in, VipsImage** out,
                                  int width, int height,
                                  unsigned int level)
{
  *out = NULL;

  double scale = 1;
	for( unsigned int l = 1; l <= level; l++ )
		scale *= 2;

  int dx = shift_x.get()/scale;
  int dy = shift_y.get()/scale;

  bool expand_x = false;
  bool expand_y = false;

  if( (dx > 0) || ((in->Xsize+dx) < width) )  expand_x = true;
  if( (dy > 0) || ((in->Ysize+dy) < height) ) expand_y = true;

  VipsImage* image = in;
  if( expand_x || expand_y ) {
    int dxabs = (dx>0) ? dx : -dx;
    int dyabs = (dy>0) ? dy : -dy;
    
    int embed_width =  in->Xsize + dxabs;
    if( width > embed_width ) embed_width = width;
    int embed_height = in->Ysize + dyabs;
    if( height > embed_height ) embed_height = height;

    int dx2 = (dx>0) ? dx : 0;
    int dy2 = (dy>0) ? dy : 0;
    
    //std::cout<<"in->Xsize="<<in->Xsize<<"  in->Ysize="<<in->Ysize<<std::endl;
    //std::cout<<"vips_embed(in, &image, "<<dx2<<", "<<dy2<<", "<<embed_width<<", "<<embed_height<<", NULL)"<<std::endl;
    //PF_PRINT_REF( in, "BlenderPar::shift_image(): in before embed:" );
    /**/
    if( vips_embed(in, &image, dx2, dy2, embed_width, embed_height, NULL) ) {
      std::cout<<"vips_embed() failed"<<std::endl;
      return false;
    }
    /**/
    //std::cout<<"image after embed: "<<image<<std::endl;
    PF_UNREF( in, "BlenderPar::shift_image(): in unref after enbed" );
    //PF_PRINT_REF( in, "BlenderPar::shift_image(): in after embed:" );
    //std::cout<<"BlenderPar::shift_image(): Image embedded"<<std::endl;
  } else {
    //PF_REF( image, "BlenderPar::shift_image(): image ref before crop" );
  }

  VipsImage* cropped = image;
  if( (dx < 0) || (dy < 0) || 
      (image->Xsize > width) ||
      (image->Ysize > height) ) {
    int dx2 = (dx<0) ? -dx : 0;
    int dy2 = (dy<0) ? -dy : 0;
    
    //std::cout<<"image->Xsize="<<image->Xsize<<"  image->Ysize="<<image->Ysize<<std::endl;
    //std::cout<<"vips_crop(image, &cropped, "<<dx2<<", "<<dy2<<", "<<width<<", "<<height<<", NULL)"<<std::endl;
    //PF_PRINT_REF( image, "BlenderPar::shift_image(): image before crop:" );
    if( vips_crop(image, &cropped, dx2, dy2, width, height, NULL) ) {
      std::cout<<"vips_crop() failed"<<std::endl;
      return false;
    }
    PF_UNREF( image, "BlenderPar::shift_image(): image unref after crop" );
    //PF_PRINT_REF( image, "BlenderPar::shift_image(): image after crop:" );
    //std::cout<<"BlenderPar::shift_image(): Image cropped"<<std::endl;
  }
  //PF_UNREF( image, "BlenderPar::shift_image(): image unref after crop" );

  *out = cropped;

  return true;
}


//#include "../vips/vips_layer.h"
int
vips_layer( VipsImage **in, int n, VipsImage **out, int first, 
            PF::ProcessorBase* proc,
            VipsImage* imap, VipsImage* omap, 
            VipsDemandStyle demand_hint,
            int width, int height, int nbands);


PF::BlenderPar::BlenderPar(): 
  OpParBase(),
  blend_mode("blend_mode",this),
  mask_blend_mode("mask_blend_mode",this),
  opacity("opacity",this,1),
  shift_x("shift_x",this,0),
  shift_y("shift_y",this,0),
  profile_bottom( NULL ),
  profile_top( NULL ),
  transform( NULL ),
  icc_data_bottom( NULL ),
  icc_data_top( NULL )
{
  white = PF::new_operation( "uniform", NULL );
  PropertyBase* R = white->get_par()->get_property( "R" );
  if( R ) R->update( (float)1 );
  PropertyBase* G = white->get_par()->get_property( "G" );
  if( G ) G->update( (float)1 );
  PropertyBase* B = white->get_par()->get_property( "B" );
  if( B ) B->update( (float)1 );

  img2lab = new PF::ICCTransform;
  lab2img = new PF::ICCTransform;

  //blend_mode.set_enum_value( PF_BLEND_PASSTHROUGH );
  blend_mode.set_enum_value( PF_BLEND_NORMAL );
  blend_mode.store_default();
  mask_blend_mode.set_enum_value( PF_MASK_BLEND_NORMAL );
  mask_blend_mode.store_default();
  set_type( "blender" );
}


PF::BlenderPar::~BlenderPar()
{
  if( white ) delete white;
  if( img2lab ) delete img2lab;
  if( lab2img) delete lab2img;
}


void PF::BlenderPar::finalize()
{
  if( get_file_format_version() > 4 ) return;

  std::cout<<std::endl<<std::endl<<std::endl<<std::endl;
  std::cout<<"BlenderPar::finalize() called"<<std::endl;
  std::cout<<std::endl<<std::endl<<std::endl<<std::endl;

  switch( blend_mode.get_enum_value().first ) {
  case PF_BLEND_MULTIPLY:
    mask_blend_mode.set_enum_value( PF_MASK_BLEND_MULTIPLY );
    break;
  case PF_BLEND_LIGHTEN:
    mask_blend_mode.set_enum_value( PF_MASK_BLEND_UNION );
    break;
  case PF_BLEND_DARKEN:
    mask_blend_mode.set_enum_value( PF_MASK_BLEND_INTERSECTION );
    break;
  default:
    mask_blend_mode.set_enum_value( PF_MASK_BLEND_NORMAL );
    break;
  }
}


VipsImage* PF::BlenderPar::build(std::vector<VipsImage*>& in, int first, 
                                 VipsImage* imap, VipsImage* omap, 
                                 unsigned int& level)
{
  VipsImage* outnew;
  VipsImage* background = NULL;
  VipsImage* foreground = NULL;
  void *prof_data_bottom;
  size_t prof_data_length_bottom;
  void *prof_data_top;
  size_t prof_data_length_top;
  //cmsHPROFILE profile_in;

  if( in.empty() ) return NULL;
  if( in.size() > 0 ) background = in[0];
  if( in.size() > 1 ) foreground = in[1];


  //PF_REF(background, "BlenderPar::build(): initial background ref");
  PF_REF(foreground, "BlenderPar::build(): initial foreground ref");
  PF_REF(omap, "BlenderPar::build(): initial omap ref");

  //if( profile_bottom ) cmsCloseProfile( profile_bottom );
  //if( profile_top ) cmsCloseProfile( profile_top );
  profile_bottom = NULL;
  profile_top = NULL;
  icc_data_bottom = NULL;
  icc_data_top = NULL;

  //std::cout<<"BlenderPar::build(): background="<<background<<std::endl;
  //if(background) {std::cout<<"bottom profile: "; PF::print_embedded_profile(background);}
  //if(foreground) {std::cout<<"top profile:    "; PF::print_embedded_profile(foreground);}
  if( !is_map() && background ) {
    // Colorspace data of background and foreground layers are compared.
    // If they differ, an explicit ICC transform is operated from foreground to background.
    // Colorspace data is ignored for map layers, which are not colormanaged.
    icc_data_bottom = PF::get_icc_profile( background );
    //std::cout<<"BlenderPar::build(): icc_data_bottom="<<icc_data_bottom<<std::endl;

    if( foreground ) {
      icc_data_top = PF::get_icc_profile( foreground );
      //std::cout<<"BlenderPar::build(): icc_data_top="<<icc_data_top<<std::endl;
      if( icc_data_bottom != icc_data_top ) {
        //if(background) {std::cout<<"bottom profile: "; PF::print_embedded_profile(background);}
        //if(foreground) {std::cout<<"top profile:    "; PF::print_embedded_profile(foreground);}
        if( icc_data_bottom ) profile_bottom = icc_data_bottom->get_profile();
        if( icc_data_top ) profile_top = icc_data_top->get_profile();
      }
    }
  }

  if( transform ) cmsDeleteTransform( transform );
  transform = NULL;
  if( profile_bottom && profile_top ) {
    cmsUInt32Number infmt = vips2lcms_pixel_format( background->BandFmt, profile_top );
    cmsUInt32Number outfmt = vips2lcms_pixel_format( foreground->BandFmt, profile_bottom );

    transform = cmsCreateTransform( profile_top, infmt,
        profile_bottom, outfmt,
        INTENT_RELATIVE_COLORIMETRIC,
        cmsFLAGS_NOOPTIMIZE | cmsFLAGS_NOCACHE );

    //std::cout<<"BlenderPar::build(): blending images with different ICC profiles"<<std::endl;
  }


  bool same_size = true;
  if( background && foreground ) {
    if( background->Xsize != foreground->Xsize) same_size = false;
    if( background->Ysize != foreground->Ysize) same_size = false;
  }

  bool do_shift = false;
  if( shift_x.get() != 0 ) do_shift = true;
  if( shift_y.get() != 0 ) do_shift = true;

  //std::cout<<"BlenderPar::build(): opacity="<<get_opacity()<<std::endl;

  /**/
  // Prepare the blending step between the new image (in invec[1]) and the underlying image
  // if existing (in invec[0]).
  // The blending code will simply force the mode to "passthrough" and copy invec[1] to outnew
  // if invec[0] is NULL
  // The mode will be forced to passthrough also when the blending mode is set to "normal",
  // the opacity is 100% and the opacity map is NULL, since in this case the underlying
  // image is useless. This improves performance for the default layer settings.
  bool is_passthrough = false;
  int cur_blend_mode = is_map() ? get_mask_blend_mode() : get_blend_mode();
  if( cur_blend_mode == PF_BLEND_PASSTHROUGH ) is_passthrough = true;
  if( (cur_blend_mode == PF_BLEND_NORMAL) &&
      (get_opacity() > 0.999999f) &&
      same_size && (do_shift == false) &&
      (omap == NULL) && (transform == NULL) ) is_passthrough = true;

  if( background && foreground && (background->Bands != foreground->Bands) )
    is_passthrough = true;
  if( background && foreground && (background->BandFmt != foreground->BandFmt) )
    is_passthrough = true;

  if( is_passthrough && (cur_blend_mode != PF_BLEND_PASSTHROUGH) ) {
    switch( get_colorspace() ) {
    case PF_COLORSPACE_RGB:
      if( get_rgb_target_channel()>= 0 )
        is_passthrough = false;
      break;
    case PF_COLORSPACE_LAB:
      if( get_lab_target_channel()>= 0 )
        is_passthrough = false;
      break;
    case PF_COLORSPACE_CMYK:
      if( get_cmyk_target_channel()>= 0 )
        is_passthrough = false;
      break;
    default:
      break;
    }
  }
  //std::cout<<"BlenderPar::build(): is_passthrough="<<is_passthrough<<std::endl;

  // If both images are not NULL and the blending mode is not "passthrough-equivalent",
  // we activate the blending code.
  // In all other cases, one of the input images is copied to the output
  // without further processing (and without any performance overhead)
  if( (background != NULL) && (foreground != NULL) && (!is_passthrough) ) {
    VipsImage* foreground2 = foreground;
    VipsImage* omap2 = omap;

    if( do_shift || (!same_size) ) {

      adjust_geom( foreground, &foreground2,
                   background->Xsize, background->Ysize, level );

      VipsImage* omap_in = omap;
      if( !omap_in ) {
        std::vector<VipsImage*> in2;
        white->get_par()->grayscale_image( foreground->Xsize, foreground->Ysize );
        white->get_par()->set_format( get_format() );
        unsigned int level2 = level;
        VipsImage* whitemask = white->get_par()->build( in2, 0, NULL, NULL, level2 );
        omap_in = whitemask;
      }
#ifndef NDEBUG
      std::cout<<"omap_in->Xsize="<<omap_in->Xsize<<"    omap_in->Ysize="<<omap_in->Ysize<<std::endl;
#endif
      adjust_geom( omap_in, &omap2,
                   background->Xsize, background->Ysize, level );
    }

    int ih_comp = image_hierarchy_compare_images(foreground2, background);
    //ih_comp = 0;
#ifndef NDEBUG
    std::cout<<"PF::BlenderPar::build(): ih_comp="<<ih_comp<<std::endl;
#endif

    std::vector<VipsImage*> in_;
    if( ih_comp == 1 ) {
#ifndef NDEBUG
      std::cout<<"PF::BlenderPar::build(): processing background before foreground"<<std::endl;
#endif
      in_.push_back( background ); bgd_id = 0;
      in_.push_back( foreground2 ); fgd_id = 1;
    } else {
#ifndef NDEBUG
      std::cout<<"PF::BlenderPar::build(): processing foreground before background"<<std::endl;
#endif
      in_.push_back( foreground2 ); fgd_id = 0;
      in_.push_back( background ); bgd_id = 1;
    }

#ifndef NDEBUG
    std::cout<<"background("<<background<<")->Xsize="<<background->Xsize<<"    background->Ysize="<<background->Ysize<<std::endl;
    std::cout<<"foreground("<<foreground<<")->Xsize="<<foreground->Xsize<<"    foreground->Ysize="<<foreground->Ysize<<std::endl;
    std::cout<<"foreground2("<<foreground2<<")->Xsize="<<foreground2->Xsize<<"    foreground2->Ysize="<<foreground2->Ysize<<std::endl;
    if( omap ) std::cout<<"omap("<<omap<<")->Xsize="<<omap->Xsize<<"    omap->Ysize="<<omap->Ysize<<std::endl;
    if( omap2 ) std::cout<<"omap2("<<omap2<<")->Xsize="<<omap2->Xsize<<"    omap2->Ysize="<<omap2->Ysize<<std::endl;
#endif

    set_image_hints( background );
    outnew = PF::OpParBase::build( in_, first, NULL, omap2, level );
    PF::image_hierarchy_fill( outnew, 0, in_ );
    //if( foreground2 != foreground ) PF_UNREF( foreground2, "BlenderPar::build(): foreground2 unref" );
    //if( omap2 != omap )
      PF_UNREF( omap2, "BlenderPar::build(): omap2 unref" );
    //PF_UNREF( background, "BlenderPar::build() background unref" );
    //PF_UNREF( foreground, "BlenderPar::build() foreground unref" );
    PF_UNREF( foreground2, "BlenderPar::build() foreground2 unref" );
    if( outnew && icc_data_bottom ) PF::set_icc_profile( outnew, icc_data_bottom );
    //std::cout<<"BlenderPar::build(): doing explicit layer blending"<<std::endl;
  } else if( (background != NULL) && (foreground != NULL) && is_passthrough ) {
    outnew = foreground;
    //PF_REF( outnew, "BlenderPar::build() foreground ref" );
    //PF_UNREF( background, "BlenderPar::build() background unref" );
  } else if( (background == NULL) && (foreground != NULL) ) {
    // background is NULL, force mode to PASSTHROUGH and copy foreground to output
    outnew = foreground;
    //PF_REF( outnew, "BlenderPar::build() foreground ref" );
  } else if( (foreground == NULL) && (background != NULL) ) {
    // foreground is NULL, force mode to PASSTHROUGH and copy background to output
    outnew = background;
    PF_REF( outnew, "BlenderPar::build() background ref" );
  } else {
    std::cerr<<"PF::BlenderPar::build(): unsupported input pattern and blend mode combination!"<<std::endl;
    return NULL;
  }
  /**/

  if( background ) {
    img2lab->init(icc_data_bottom, PF::ICCStore::Instance().get_Lab_profile(),
        background->BandFmt, INTENT_RELATIVE_COLORIMETRIC, true, 0);
    lab2img->init(PF::ICCStore::Instance().get_Lab_profile(), icc_data_bottom,
        background->BandFmt, INTENT_RELATIVE_COLORIMETRIC, true, 0);
  }

  //VipsImage* invec[2] = {background, foreground};
  //vips_layer( invec, 2, &outnew, 0, get_processor(), NULL, omap, get_demand_hint() );
#ifndef NDEBUG
  std::cout<<"PF::BlenderPar::build(): input: "<<background<<" "<<foreground<<"   output: "<<outnew<<std::endl;
#endif
  //set_image( outnew );
#ifndef NDEBUG
  if( outnew ) {
    //std::cout<<"BlenderPar::build(): Output profile: "<<std::endl;
    PF::print_embedded_profile(outnew);
  }
#endif

  if( outnew && get_output_caching() ) {
    int p = get_output_padding( 0 );
    if( false && p > 0 ) {
      int nt = outnew->Xsize*(p/PF_OUPUT_CACHE_TS + 3)/PF_OUPUT_CACHE_TS;
      VipsAccess acc = VIPS_ACCESS_SEQUENTIAL;
      //VipsAccess acc = VIPS_ACCESS_RANDOM; // gives worst performances
      int threaded = 1, persistent = 0;
      VipsImage* cached;
      if( !vips_tilecache(outnew, &cached,
          "tile_width", PF_OUPUT_CACHE_TS,
          "tile_height", PF_OUPUT_CACHE_TS,
          "max_tiles", nt,
          "access", acc, "threaded", threaded,
          "persistent", persistent, NULL) ) {
        PF_UNREF( outnew, "BlenderPar::build(): outnew unref" );
        outnew = cached;
        std::cout<<"BlenderPar::build(): added tilecache for output image, padding="<<p<<", nt="<<nt<<std::endl;
      }
    }
  }

  return outnew;
}
