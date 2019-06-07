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

#ifndef PF_SHAHI_V2_H
#define PF_SHAHI_V2_H


#include "../base/processor.hh"

#define SH_LOG_SCALE_MIN (-2.0f)
#define SH_LOG_SCALE_MAX (1.0f)

namespace PF
{


class RelightPar: public OpParBase
{
  Property<float> strength, range, contrast, LE_compression;

  ProcessorBase* shahl;
  ProcessorBase* tm;

public:
  RelightPar();

  bool has_intensity() { return false; }
  bool needs_caching();

  void compute_padding( VipsImage* full_res, unsigned int id, unsigned int level );
  void propagate_settings();

  void pre_build(rendermode_t mode);
  VipsImage* build(std::vector<VipsImage*>& in, int first,
      VipsImage* imap, VipsImage* omap,
      unsigned int& level);
};



template < OP_TEMPLATE_DEF >
class RelightProc
{
public:
  void render(VipsRegion** in, int n, int in_first,
      VipsRegion* imap, VipsRegion* omap,
      VipsRegion* out, OpParBase* par)
  {
  }
};


ProcessorBase* new_relight();

}

#endif 

