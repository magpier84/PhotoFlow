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

//#include "../../../operations/gmic/smooth_wavelets_haar.hh"

#include "smooth_wavelets_haar_config.hh"


PF::GmicSmoothWaveletsHaarConfigDialog::GmicSmoothWaveletsHaarConfigDialog( PF::Layer* layer ):
  OperationConfigDialog( layer, "Smooth [wavelets] (G'MIC)"  ),
  //iterations_slider( this, "iterations", "Iterations", 1, 1, 10, 1, 1, 1),
  prop_threshold_slider( this, "threshold", "threshold", 1, 0, 10, .10, 1.0, 1),
  prop_iterations_slider( this, "iterations", "iterations", 10, 1, 32, 1, 5, 1),
  prop_scales_slider( this, "scales", "scales", 10, 2, 10, 1, 5, 1)
{
  //controlsBox.pack_start( iterations_slider );
  controlsBox.pack_start( prop_threshold_slider );
  controlsBox.pack_start( prop_iterations_slider );
  controlsBox.pack_start( prop_scales_slider );
  
  add_widget( controlsBox );
}



void PF::GmicSmoothWaveletsHaarConfigDialog::open()
{
  OperationConfigDialog::open();
}
