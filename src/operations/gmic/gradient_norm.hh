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

#ifndef GMIC_GRADIENT_NORM_H
#define GMIC_GRADIENT_NORM_H


#include "../base/processor.hh"


namespace PF 
{

  class GmicGradientNormPar: public OpParBase
  {
    Property<int> iterations;
    Property<float> prop_smoothness;
    Property<float> prop_linearity;
    Property<float> prop_min_threshold;
    Property<float> prop_max_threshold;
    ProcessorBase* gmic;

  public:
    GmicGradientNormPar();

    bool has_intensity() { return false; }
    bool has_opacity() { return true; }
    bool needs_caching() { return false; }


    int get_gmic_padding( int level );

    void compute_padding( VipsImage* full_res, unsigned int id, unsigned int level )
    {
      set_padding( get_gmic_padding(level), id);
    }


    VipsImage* build(std::vector<VipsImage*>& in, int first, 
                     VipsImage* imap, VipsImage* omap, 
                     unsigned int& level);
  };

  

  template < OP_TEMPLATE_DEF > 
  class GmicGradientNormProc
  {
  public: 
    void render(VipsRegion** ireg, int n, int in_first,
                VipsRegion* imap, VipsRegion* omap, 
                VipsRegion* oreg, OpParBase* par)
    {	
    }
  };




  ProcessorBase* new_gmic_gradient_norm();
}

#endif 


