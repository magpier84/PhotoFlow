#! /bin/bash


dtdir="$1"
cp "$dtdir/src/external/adobe_coeff.c" "$dtdir/src/external/wb_presets.c" src/dt/external
cp "$dtdir/src/common/colormatrices.c" src/dt/common
cp "$dtdir/src/common/srgb_tone_curve_values.h" src/dt/common
#"$dtdir/src/common/colorspaces.h"
#"$dtdir/src/common/colorspaces.c"
