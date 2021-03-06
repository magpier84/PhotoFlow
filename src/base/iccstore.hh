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

#ifndef PF_ICCSTORE_H
#define PF_ICCSTORE_H

#include <string.h>
#include <string>
#include <lcms2.h>
#include <glibmm.h>
#include <vips/vips.h>

#include "color.hh"
#include "../rt/rtengine/LUT.h"

namespace PF
{

enum TRC_type
{
  PF_TRC_STANDARD=0,
  PF_TRC_PERCEPTUAL=1,
  PF_TRC_LINEAR=2,
  PF_TRC_sRGB=3,
  PF_TRC_GAMMA_22=4,
  PF_TRC_GAMMA_18=5,
  PF_TRC_UNKNOWN=1000
};


enum profile_mode_t {
  PROF_MODE_NONE,
  PROF_MODE_DEFAULT,
  PROF_MODE_EMBEDDED,
  PROF_MODE_EMBEDDED_sRGB,
  PROF_MODE_CUSTOM,
  PROF_MODE_ICC
};


enum profile_type_t {
  PROF_TYPE_EMBEDDED,
  PROF_TYPE_FROM_SETTINGS,
  PROF_TYPE_FROM_DISK,
  PROF_TYPE_sRGB,
  PROF_TYPE_sRGB_D50,
  PROF_TYPE_REC2020,
  PROF_TYPE_ADOBE,
  PROF_TYPE_PROPHOTO,
  PROF_TYPE_PROPHOTO_D65,
  PROF_TYPE_ACEScg,
  PROF_TYPE_ACES,
  PROF_TYPE_LAB,
  PROF_TYPE_XYZ,
  PROF_TYPE_CUSTOM,
  PROF_TYPE_NONE,
};



extern cmsToneCurve* Lstar_trc;
extern cmsToneCurve* iLstar_trc;


struct ICCProfileData
{
  cmsToneCurve* perceptual_trc;
  cmsToneCurve* perceptual_trc_inv;
  TRC_type trc_type;

  //float perceptual_trc_vec[65536];
  //float perceptual_trc_inv_vec[65536];

  float Y_R, Y_G, Y_B;
};


#define PF_GAMUT_MAP_NJZ 500

class ICCTransform;

class ICCProfile
{
  bool has_colorants;
  void* profile_data;
  cmsUInt32Number profile_size;
  cmsHPROFILE profile;
  cmsToneCurve* perceptual_trc;
  cmsToneCurve* perceptual_trc_inv;
  profile_type_t profile_type;
  TRC_type trc_type;
  bool parametric_trc;

  Glib::ustring filename;

  //float perceptual_trc_vec[65536];
  //float perceptual_trc_inv_vec[65536];

  LUTf p2l_lut, l2p_lut;

  double colorants[9];
  float rgb2xyz[3][3];
  float rgb2xyz100_D65[3][3];
  float xyz1002rgb_D65[3][3];
  float Y_R, Y_G, Y_B;

  void init_trc( cmsToneCurve* trc, cmsToneCurve* trc_inv );

  int refcount;

  ICCTransform* to_lab;
  ICCTransform* from_lab;

  float** gamut_boundary;
  float** gamut_boundary_out;
  float* gamut_Lid_Cmax;


public:
  ICCProfile();
  virtual ~ICCProfile();

  void ref() { refcount += 1; }
  void unref() { refcount -= 1; }
  int get_ref_count() { return refcount; }

  void set_file_name( Glib::ustring name ) { filename = name; }
  Glib::ustring get_file_name() { return filename; }

  void init_colorants();
  void init_trc();
  void init_Lab_conversions( ICCProfile* plab );

  bool is_rgb();
  bool is_grayscale();
  bool is_lab();
  bool is_cmyk();

  bool is_matrix();
  double* get_colorants() { return colorants; }

  void set_profile_type(profile_type_t p) { profile_type = p; }
  profile_type_t get_profile_type() { return profile_type; }
  void set_trc_type(TRC_type type) { trc_type = type; }
  TRC_type get_trc_type() { return trc_type; }
  bool is_linear() { return( get_trc_type() == PF_TRC_LINEAR ); }
  bool is_parametric() { return( parametric_trc ); }
  bool is_perceptual() { return( get_trc_type() == PF_TRC_PERCEPTUAL ); }
  bool is_standard() { return( get_trc_type() == PF_TRC_STANDARD ); }

  cmsToneCurve* get_p2l_trc() { return perceptual_trc; }
  cmsToneCurve* get_l2p_trc() { return perceptual_trc_inv; }

  void set_profile( cmsHPROFILE p );
  cmsHPROFILE get_profile(); //{ return profile; }

  cmsUInt32Number get_profile_size() { return profile_size; }
  void* get_profile_data() { return profile_data; }

  cmsFloat32Number linear2perceptual( cmsFloat32Number val );
  cmsFloat32Number perceptual2linear( cmsFloat32Number val );

  //float* get_linear2perceptual_vec() { return perceptual_trc_inv_vec; }
  //float* get_perceptual2linear_vec() { return perceptual_trc_vec; }

  float get_luminance( const float& R, const float& G, const float& B )
  {
    return( (has_colorants) ? Y_R*R + Y_G*G + Y_B*B : 0.0f );
  }
  //void get_luminance( float* RGBv, float* Lv, size_t size );

  float get_lightness( const float& R, const float& G, const float& B );
  void get_lightness( float* RGBv, float* Lv, size_t size );

  void to_Jzazbz( const float& R, const float& G, const float& B, float& Jz, float& az, float& bz );
  void from_Jzazbz( const float& Jz, const float& az, const float& bz, float& R, float& G, float& B );

  void init_gamut_mapping();
  void set_destination_gamut(ICCProfile* pout);
  float** get_gamut_boundary() { return gamut_boundary; }
  float* get_gamut_Lid_Cmax() { return gamut_Lid_Cmax; }
  void gamut_mapping( float& R, float& G, float& B, float** gamut_boundary_out, float* gamut_Lid_Cmax_out, float saturation );
  bool chroma_compression( float& J, float& C, float& H, float** gamut_boundary_out, float* gamut_Lid_Cmax_out, float saturation );

  bool equals_to( PF::ICCProfile* prof);

  ICCProfileData* get_data()
  {
    ICCProfileData* data = new ICCProfileData;
    data->trc_type = trc_type;
    //memcpy( data->perceptual_trc_vec, perceptual_trc_vec, sizeof(perceptual_trc_vec) );
    //memcpy( data->perceptual_trc_inv_vec, perceptual_trc_inv_vec, sizeof(perceptual_trc_inv_vec) );
    data->perceptual_trc =  cmsDupToneCurve( perceptual_trc );
    data->perceptual_trc_inv =  cmsDupToneCurve( perceptual_trc_inv );
    data->Y_R = Y_R;
    data->Y_G = Y_G;
    data->Y_B = Y_B;

    return data;
  }
};


//ICCProfileData* get_icc_profile_data( VipsImage* img );
//void free_icc_profile_data( ICCProfileData* data );
void set_icc_profile( VipsImage* img, ICCProfile* prof );
ICCProfile* get_icc_profile( VipsImage* img );
void iccprofile_unref( void* prof );

//cmsFloat32Number linear2perceptual( ICCProfileData* data, cmsFloat32Number val );
//cmsFloat32Number perceptual2linear( ICCProfileData* data, cmsFloat32Number val );


class DiskProfile: public ICCProfile
{
public:
  DiskProfile();
};


class ACESProfile: public ICCProfile
{
public:
  ACESProfile(TRC_type type);
};


class Rec2020Profile: public ICCProfile
{
public:
  Rec2020Profile(TRC_type type);
};


class sRGBProfile: public ICCProfile
{
public:
  sRGBProfile(TRC_type type);
};


class sRGBProfileD50: public ICCProfile
{
public:
  sRGBProfileD50(TRC_type type);
};


class ProPhotoProfile: public ICCProfile
{
public:
  ProPhotoProfile(TRC_type type);
};


class ProPhotoProfileD65: public ICCProfile
{
public:
  ProPhotoProfileD65(TRC_type type);
};


class LabProfile: public ICCProfile
{
public:
  LabProfile(TRC_type type);
};


class XYZProfile: public ICCProfile
{
public:
  XYZProfile(TRC_type type);
};


class ICCTransform
{
  ICCProfile* in_profile;
  ICCProfile* out_profile;
  cmsUInt32Number intent;
  bool bpc;
  float adaptation_state;
  cmsHTRANSFORM transform;

  LUTf trc_lut, itrc_lut;

  bool is_rgb2rgb;
  float rgb2rgb[3][3];

  cmsColorSpaceSignature input_cs_type;
  cmsColorSpaceSignature output_cs_type;

public:
  ICCTransform(): in_profile(NULL), out_profile(NULL), transform(NULL) {}
  ~ICCTransform() {
    if( transform ) {
      cmsDeleteTransform( transform );
    }
  }
  bool valid() { return( transform!=NULL || is_rgb2rgb ); }
  void init(ICCProfile* in_profile, ICCProfile* out_profile, VipsBandFormat band_fmt,
      cmsUInt32Number intent=INTENT_RELATIVE_COLORIMETRIC,
      bool bpc=true, float adaptation_state=0);
  void apply(float* in, float* out, int n=1);
};


class ICCStore
{
  ICCProfile* srgb_profiles[6];
  ICCProfile* srgb_d50_profiles[6];
  ICCProfile* rec2020_profiles[6];
  ICCProfile* aces_profiles[6];
  ICCProfile* acescg_profiles[6];
  ICCProfile* adobe_profiles[6];
  ICCProfile* prophoto_profiles[6];
  ICCProfile* prophoto_d65_profiles[6];
  ICCProfile* system_monitor_profile;
  std::vector<ICCProfile*> profiles;

  ICCProfile* Lab_profile;
  ICCProfile* XYZ_profile;

  static ICCStore* instance;

  Glib::ustring    defaultMonitorProfile;  // Main monitors standard profile name, from OS
public:
  ICCStore();

  static ICCStore& Instance();

  ICCProfile* get_srgb_profile(TRC_type type) { return srgb_profiles[type]; }
  ICCProfile* get_profile(profile_type_t ptype, TRC_type trc_type);
  ICCProfile* get_profile( Glib::ustring pname );
  ICCProfile* get_profile( void* pdata, cmsUInt32Number psize );
  ICCProfile* get_profile( cmsHPROFILE profile );

  ICCProfile* get_Lab_profile() { return Lab_profile; }
  ICCProfile* get_XYZ_profile() { return XYZ_profile; }

  cmsToneCurve* get_Lstar_trc() {return Lstar_trc; }
  cmsToneCurve* get_iLstar_trc() {return iLstar_trc; }

  void set_system_monitor_profile( cmsHPROFILE profile );
  ICCProfile* get_system_monitor_profile() { return system_monitor_profile; }
};
}


#endif 


