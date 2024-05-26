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

APTR  AllocFBMem(LONG fbNum, LONG width, LONG height, LONG depth);
APTR  FreeFBMem(APTR fbAdr);
VOID  DisplayOn(APTR fbAdr, LONG displayModes);
VOID  DisplayOff(APTR fbAdr);
VOID  WritePixRGB(APTR fbAdr, LONG x, LONG y, ULONG rgbColor);
ULONG ReadPixRGB(APTR fbAdr, LONG x, LONG y);

#endif
