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


#include "gmic.hh"
#include "iain_denoise.hh"



PF::GmicIainDenoisePar::GmicIainDenoisePar(): 
OpParBase(),
  iterations("iterations",this,1),
  prop_luma("luma",this,3),
  prop_chroma("chroma",this,3),
  prop_despeckle("despeckle",this,1),
  prop_highlights("highlights",this,0),
  prop_shadows("shadows",this,0),
  prop_recover_details("recover_details", this, 0, "Do_not_Recover_Details", "Do not Recover Details"),
  prop_recovery_amount("recovery_amount",this,0.1),
  prop_adjust_fine_details("adjust_fine_details",this,0),
  prop_adjust_medium_details("adjust_medium_details",this,0),
  prop_adjust_large_details("adjust_large_details",this,0),
  prop_detail_emphasis("detail_emphasis",this,1.35),
  prop_sharpen_edges("sharpen_edges",this,0)
{	
  gmic = PF::new_gmic();
  prop_recover_details.add_enum_value( 1, "Recover_Details", "Recover Details" );
  set_type( "gmic_iain_denoise" );
}


int PF::GmicIainDenoisePar::get_gmic_padding( int level )
{
  return 5;
}


VipsImage* PF::GmicIainDenoisePar::build(std::vector<VipsImage*>& in, int first, 
                                        VipsImage* imap, VipsImage* omap, 
                                        unsigned int& level)
{
  VipsImage* srcimg = NULL;
  if( in.size() > 0 ) srcimg = in[0];
  VipsImage* mask;
  VipsImage* out = srcimg;

  if( !out ) return NULL;
  
  if( !(gmic->get_par()) ) return NULL;
  PF::GMicPar* gpar = dynamic_cast<PF::GMicPar*>( gmic->get_par() );
  if( !gpar ) return NULL;

  float scalefac = 1;
	for( unsigned int l = 1; l <= level; l++ )
		scalefac *= 2;

  std::string command = "-iain_iains_nr  ";
  command = command + prop_luma.get_str();
  command = command + std::string(",") + prop_chroma.get_str();
  command = command + std::string(",") + prop_despeckle.get_str();
  command = command + std::string(",") + prop_highlights.get_str();
  command = command + std::string(",") + prop_shadows.get_str();
  command = command + std::string(",") + prop_recover_details.get_enum_value_str();
  command = command + std::string(",") + prop_recovery_amount.get_str();
  command = command + std::string(",") + prop_adjust_fine_details.get_str();
  command = command + std::string(",") + prop_adjust_medium_details.get_str();
  command = command + std::string(",") + prop_adjust_large_details.get_str();
  command = command + std::string(",") + prop_detail_emphasis.get_str();
  command = command + std::string(",") + prop_sharpen_edges.get_str();
  gpar->set_command( command.c_str() );
  gpar->set_iterations( iterations.get() );
  gpar->set_gmic_padding( get_gmic_padding( level ) );
  gpar->set_x_scale( 1.0f );
  gpar->set_y_scale( 1.0f );

  gpar->set_image_hints( srcimg );
  gpar->set_format( get_format() );

  out = gpar->build( in, first, imap, omap, level );
  if( !out ) {
    std::cout<<"gmic.build() failed!!!!!!!"<<std::endl;
  }

	return out;
}


PF::ProcessorBase* PF::new_gmic_iain_denoise()
{
  return( new PF::Processor<PF::GmicIainDenoisePar,PF::GmicIainDenoiseProc>() );
}
