#ifndef CLIB_PICOGFX_PROTOS_H
#define CLIB_PICOGFX_PROTOS_H

/**
 **     $Filename: clib/picogfx_protos.h $
 **     $Release: 0 $
 **     $Revision: 0.0 $
 **     $Date: 14 Jan 1992 $
 **
 **     copyright (c) 1992 Hertz Elektronik GmbH
 **     all rights reserved
 **/

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

APTR __stdargs AllocFBMem(LONG fbNum, LONG width, LONG height, LONG depth);
APTR __stdargs FreeFBMem(APTR fbAdr);
VOID __stdargs DisplayOn(APTR fbAdr, LONG displayModes);
VOID __stdargs DisplayOff(APTR fbAdr);
VOID __stdargs WritePixRGB(APTR fbAdr, LONG x, LONG y, ULONG rgbColor);
ULONG __stdargs ReadPixRGB(APTR fbAdr, LONG x, LONG y);

#endif
