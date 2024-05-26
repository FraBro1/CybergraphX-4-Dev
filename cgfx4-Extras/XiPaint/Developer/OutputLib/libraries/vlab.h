#ifndef VLAB_H
#define VLAB_H 1

/* VLab - include. By Thomas Dorn from Macro Systems */

#define VMMODE_RETINA  1


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

/* ;--------------------------------------------------------------------------- */
/* ; function offsets for Custom(); default value in []. */

#define VLREG_NOP            0      /* ;NOP */
#define VLREG_LUMINANCE      1      /* ;luminance filter            0=OFF, 1=ON  [0] */
#define VLREG_CHROMINANCE    2      /* ;chrominance filter          0=OFF, 1=ON  [1] */
#define VLREG_VTR            3      /* ;timebase-correction         0=OFF, 1=ON  [0] */
#define VLREG_PAL            4      /* ;PAL/NTSC switch             0=OFF, 1=ON  [1] */
#define VLREG_VSC            5      /* ;VSync-correction            0=OFF, 1=ON  [1] */
#define VLREG_WEIGHT         6      /* ;weight of FILTER and NOISE  0=OFF, 1=LIGHT, */
	                            /* ;                            2=MEDIUM, 3=STRONG [1] */
#define VLREG_FILTER         7      /* ;filter-frency            0=OFF, 1=LIGHT, */
                                    /* ;                            2=MEDIUM, 3=STRONG [1] */
#define VLREG_NOISE          8      /* ;noise-canceller             0=OFF, 1=LIGHT, */
                                    /* ;                            2=MEDIUM, 3=STRONG [2] */
#define VLREG_HUE            9      /* ;NTSC-color-correction       0..255       [0] */
#define VLREG_FULLFRAME      10     /* ;frame select                0=OFF, 1=ON  [0] */
#define VLREG_SLOWSCAN       11     /* ;slowscan switch             0=OFF, 1=ON  [0] */
#define VLREG_INPUT          12     /* ;CVBS-input-select           0=lower, 1=upper [0] */
#define VLREG_RESERVED13     13     /* ;reserved, do not use */
#define VLREG_RESERVED14     14     /* ;reserved, do not use */
#define VLREG_CCIR           15     /* auto convert to CCIR        0=OFF, 1=ON [1] */
/* ;--------------------------------------------------------------------------- */
/* ; Modes for YUVtoRGB() */

#define YUV411_TO_LORES       0
#define YUV411_TO_HIRES       1
#define YUV211_TO_LORES       2     /* ; new in v4 of vlab.library */

/* ; the following names are obsolete, use the new names above */

#define RGB_LORES             YUV411_TO_LORES    /* ; OBSOLETE */
#define RGB_HIRES             YUV411_TO_HIRES    /* ; OBSOLETE */
/* ;--------------------------------------------------------------------------- */
/* ; Modes for VLab_DeInterlace() */
#define DIM_EVEN        0
#define DIM_ODD         1
#define DIM_MIX         2
#define DIM_SET         3


/* ;--------------------------------------------------------------------------- */
/* ; the public-part of vlab.library */

struct VLabBase
{
    struct Library xvy;
    short    vlb_MaxWidth;       /* ; max. width */
    short    vlb_MaxHeightPAL;   /* ; max. PAL-height */
    short    vlb_MaxHeightNTSC;  /* ; max. NTSC-height */
    short    vlb_InputsPerBoard; /* ; number of inputs on the VLab-board */
    short    vlb_RatioYUV;       /* ; number of Y-pixels for one UV-pixel */
    ULONG    vlb_HardInfo;       /* ;NULL, if there is no VLab hardware */
                               /* ;Non-NULL if there is a hardware. Do */
                               /* ;not assume any special value, it may */
                               /* ;change in the future! (v6) */
                               /* ;always NULL in v6, fixed in v7 */

    WORD    vlb_DIVal_Y;        /* ;use these values as a default for */
    WORD    vlb_DIVal_U;        /* ;  VLab_DeInterlace() */
    WORD    vlb_DIVal_V;        /* ; */
    WORD    vlb_DIVal_R;        /* ; */
    WORD    vlb_DIVal_G;        /* ; */
    WORD    vlb_DIVal_B;        /* ; */

};

/* ; subsequent fields in the library base are VLAB PRIVATE */
/* ;--------------------------------------------------------------------------- */
#define VLERR_OK              0   /* ; no error, everything OK */
#define VLERR_ADDRESS         1   /* ; you passed an illegal buffer-address to VLab_Scan() */
#define VLERR_NOHARD          2   /* ; hardware not found        (*) */
#define VLERR_CLIP            3   /* ; you passed an illegal clip-definition to VLab_Scan() */
#define VLERR_NOVIDEO         4   /* ; no video signal found     (*) */
#define VLERR_SCAN            5   /* ; error during scan         (*) */
#define VLERR_CUSTOM          6   /* ; illegal register or value */
#define VLERR_INIT            7   /* ; error while sending the new value to the hardware */
#define VLERR_NOMEM           8   /* ; not enough memory available  (*)  (v6) */

/* ; (*) Please notify the user with a reasonable text if one of the */
/* ;     marked errors occured! */

/* ;--------------------------------------------------------------------------- */



#define VLAB_LIB_VERSION  7                  /* ;the current version of vlab.library */

/* ; Modes for VLab_DeInterlace() */
#define DIM_FRAME1    0
#define DIM_FRAME2    1
#define DIM_MIX       2

/* ; suggested values for deinterlacing various data types */
#define DIVAL_Y       10
#define DIVAL_U       30
#define DIVAL_V       30
#define DIVAL_R       20
#define DIVAL_G       10
#define DIVAL_B       40


/* ;--------------------------------------------------------------------------- */
/* ; Modes for VLab_Convert */
#define CM_CCIR_Y      0
#define CM_CCIR_UV     1
/* ;--------------------------------------------------------------------------- */
/* ; Modes for VLab_Monitor */
#define VMMODE_RETINA_8    equ   0  /* ; Dest is an 8 bit Retina screen. */
#define VMMODE_RETINA_24   equ   1  /* ; Dest is a 24 bit Retina screen. */
#define VMMODE_MEMORY_8    equ   2  /* ; Dest is 8 Bit Amiga memory. */
#define VMMODE_MEMORY_RGB  equ   3  /* ; Dest is 24 Bit Amiga memory. */
#define VMMODE_MEMORY_RGBA equ   4  /* ; Dest is 32 Bit Amiga memory. */
#define VMMODE_MEMORY_GB_R equ   5  /* ; For VDPaint only, do NOT use. */
/* ;--------------------------------------------------------------------------- */
/* ; special default values for VLab_DeInterlace(): */
#define DIDVAL_Y         -1
#define DIDVAL_U         -2
#define DIDVAL_V         -3
#define DIDVAL_R         -4
#define DIDVAL_G         -5
#define DIDVAL_B         -6
/* ;--------------------------------------------------------------------------- */
/* ; function offsets for Custom(); default value in []. */
