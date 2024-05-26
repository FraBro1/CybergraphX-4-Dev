#ifndef DATATYPES_PICTURECLASSEXT_H
#define DATATYPES_PICTURECLASSEXT_H 1

/*
**  $VER: pictureclassext.h 43.2 (9.3.97)
**
**  Extended V43 interface definitions for DataType picture objects.
**
**  Written by Ralph Schmidt, Frank Mariak and Roland Mainz
**
*/


#ifndef DATATYPES_DATATYPESCLASS_H
#include <datatypes/datatypesclass.h>
#endif /* !DATATYPES_DATATYPESCLASS_H */


#define	PDTA_SourceMode		(DTA_Dummy + 250)	/* Used to set the sub datatype interface */
#define	PDTA_DestMode		(DTA_Dummy + 251)	/* Used to set the app datatype interface */
#define	PDTA_PixelFormat	(DTA_Dummy + 252)	/* private..DON'T touch */
#define	PDTA_TransRemapPen	(DTA_Dummy + 253)	/* Optional transparent remap pen */
#define	PDTA_NumPixMapDir	(DTA_Dummy + 254)	/* Count of the Object's PixMapDirs..default=1 */
#define PDTA_UseFriendBitMap	(DTA_Dummy + 255)	/* Converts the result bitmap into a friendbitmap */
							/* This will speed up rendering a lot */
#define PDTA_AlphaChannel	(DTA_Dummy + 256)	/* Alphachannel input */
#define PDTA_MultiRemap		(DTA_Dummy + 257)	/* Tells the picture.datatype NOT to keep control */
                                                        /* over DestBitmap and Pens...these are now*/
                                                        /* controlled by the appliction. TM mode*/
#define	PDTA_MaskPlane		(DTA_Dummy + 258)	/* NULL or MaskPlane for BltMaskBitMapRastPort() */
                                                        /* This mask is generated when the transparent flag is set*/
                                                        /* or an Alphachannel exists */




/* PDTA_SourceMode, PDTA_DestMode */
#define PMODE_V42        (0)
#define PMODE_V43        (1)

/* Extended V43 pictureclass methods */
#define PDTM_Dummy           (DTM_Dummy + 0x60UL) /* see datatypes/datatypesclass.h */

#define PDTM_WRITEPIXELARRAY (PDTM_Dummy + 0UL)
    /* Used to transfer pixel data to the picture.datatype object in the specified format */

#define PDTM_READPIXELARRAY  (PDTM_Dummy + 1UL)
    /* Used to transfer pixel data from the picture.datatype object in the specified format */

#define PDTM_CREATEPIXMAPDIR (PDTM_Dummy + 2UL)
    /* Used to create a new pixmap directory for multi-volume bitmap data */

#define PDTM_FIRSTPIXMAPDIR  (PDTM_Dummy + 3UL)
    /* Used to set the current pixmap to the first one in the list  */

#define PDTM_NEXTPIXMAPDIR   (PDTM_Dummy + 4UL)
    /* Used to set the current pixmap to the next one in the list  */

#define PDTM_PREVPIXMAPDIR   (PDTM_Dummy + 5UL)
    /* Used to set the current pixmap to the previous one in the list  */

#define PDTM_BESTPIXMAPDIR   (PDTM_Dummy + 6UL)
    /* Sets the pixmap directory to the best one fitted for the screen */

/* PDTM_WRITEPIXELARRAY, PDTM_READPIXELARRAY */
struct pdtBlitPixelArray
{
    ULONG  MethodID;             /* PDTM_BLITPIXELARRAY or PDTM_READPIXELARRAY id             */
    UBYTE *pbpa_PixelArray;      /* Source PixelArray                                         */
    ULONG  pbpa_PixelArrayMode;  /* Format Mode of the Source PixelArray; see cybergraphics.h */
    ULONG  pbpa_PixelArrayMod;   /* Bytes to add to the next line in the Source PixelArray    */
    ULONG  pbpa_LeftEdge;        /* XStart of the Dest                                        */
    ULONG  pbpa_TopEdge;         /* YStart of the Dest                                        */
    ULONG  pbpa_Width;           /* Width of the Source PixelArray                            */
    ULONG  pbpa_Height;          /* Height of the Source PixelArray                           */
};


/* Obsolete pictureclassext.h definitions, here for source code compatibility only.
 * Please do NOT use in new code.
 *
 * #define PICTURECLASS_NEWNAMES_ONLY to remove these older names
 */
#ifndef PICTURECLASS_NEWNAMES_ONLY

#define DTM_WRITEPIXELARRAY PDTM_WRITEPIXELARRAY
#define DTM_READPIXELARRAY  PDTM_READPIXELARRAY
#define DTM_CREATEPIXMAPDIR PDTM_CREATEPIXMAPDIR
#define DTM_FIRSTPIXMAPDIR  PDTM_FIRSTPIXMAPDIR
#define DTM_NEXTPIXMAPDIR   PDTM_NEXTPIXMAPDIR
#define DTM_PREVPIXMAPDIR   PDTM_PREVPIXMAPDIR
#define DTM_BESTPIXMAPDIR   PDTM_BESTPIXMAPDIR

/* Obsolete DTM_BLITPIXELARRAY msg, use struct pdtBlipPixelArray instead */
struct gpBlitPixelArray
{
	ULONG		MethodID;		/* DTM_BLITPIXELARRAY id */
	UBYTE		*PixelArray;		/* Source PixelArray */
	ULONG		PixelArrayMode;		/* Format Mode of the Source PixelArray..see cybergraphics.h */
	ULONG		PixelArrayMod;		/* Bytes to add to the next line in the Source PixelArray */
	ULONG		LeftEdge;		/* XStart of the Dest */
	ULONG		TopEdge;		/* YStart of the Dest */
	ULONG		Width;			/* Width of the Source PixelArray */
	ULONG		Height;			/* Height of the Source PixelArray */
};

/* PDTA_SourceMode,PDTA_DestMode */
#define	MODE_V42 PMODE_V42
#define	MODE_V43 PMODE_V43

#endif /* !PICTURECLASS_V44_NAMES_ONLY */

#endif /* !DATATYPES_PICTURECLASSEXT_H */
