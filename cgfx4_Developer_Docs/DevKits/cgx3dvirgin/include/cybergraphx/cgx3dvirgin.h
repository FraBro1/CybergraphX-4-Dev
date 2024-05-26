/*
**	$VER: cgx3dvirgin.h 3.1 (28.04.1998)
**
**	include file for cgx3dvirgin.library
**
**	Copyright © 1996-1998 by phase5 digital products
**      All Rights reserved.
**
*/

#ifndef LIBRARIES_CGX3DVIRGIN_H
#define LIBRARIES_CGX3DVIRGIN_H

typedef APTR    TexHandle;
typedef APTR    View3D;

typedef struct
{
        UBYTE a,r,g,b;
} ARGB32;

typedef union
{
        LONG    argbval;
        ARGB32  argb32;
} ARGB;


typedef struct
{
        LONG x,y,z,u,v;
        ARGB  color;
} Point3D;

typedef struct
{
        LONG x,y,z,w;
        ARGB  color;
} VertexV3Dlit;

typedef struct
{
        LONG x,y,z,w;
        ARGB color;
        LONG d,u,v;
} VertexV3Dtex;


typedef struct
{
        Point3D p1,p2,p3;
        struct TexHandle *th;
} Triangle3D;


typedef struct
{
        Point3D p1,p2;
} Line3D;

typedef struct
{
        ULONG L3D_Z_BASE;	// reserved, set by library
        ULONG L3D_DEST_BASE;	// reserved, set by library
        ULONG L3D_CLIP_L_R;	// reserved, set by library
        ULONG L3D_CLIP_T_B;	// reserved, set by library
        ULONG L3D_DEST_STR;	// reserved, set by library
        ULONG L3D_Z_STRIDE;	// reserved, set by library
        ULONG L3D_FOG_CLR;      // Fog color
                                // This is the fog color blended with the pixel
                                // color when bit 17 of the L3D_CMD_SET register
                                // is set to 1. This operation is also called
                                // depth cueing when the fog factor (source
                                // alpha) corresponds to the distance from the
                                // viewer
        ULONG L3D_CMD_SET;      // 3D command register
        ULONG L3D_3dGdY_dBdY;	// 3D line draw GB Delta
        ULONG L3D_3dAdY_dRdY;	// 3D line draw AR Delta
        ULONG L3D_3GS_BS;	// 3D line draw GB Start
        ULONG L3D_3AS_RS;	// 3D line draw AR Start
        ULONG L3D_3dZ;		// 3D line draw Z Delta
        ULONG L3D_3ZSTART;	// 3D line draw Z Start
        ULONG L3D_3XEND0_END1;	// 3D line draw Endpoints
        ULONG L3D_3dX;		// 3D line draw X Delta
        ULONG L3D_3XSTART;	// 3D line draw X Start
        ULONG L3D_3YSTART;	// 3D line draw Y Start
        ULONG L3D_3YCNT;	// 3D line draw Y Count
} V3DLine;

typedef struct 
{
        ULONG T3D_Z_BASE;       // reserved, set by library
        ULONG T3D_DEST_BASE;    // reserved, set by library
        ULONG T3D_CLIP_L_R;     // reserved, set by library
        ULONG T3D_CLIP_T_B;     // reserved, set by library
        ULONG T3D_DEST_SRC_STR; // reserved, set by library
        ULONG T3D_Z_STRIDE;     // reserved, set by library
        ULONG T3D_TEX_BASE;     // reserved, set by library
        ULONG T3D_TEX_BDR_CLR;  // reserved, set by library
        ULONG T3D_FOG_CLR;      // Fog color
                                // This is the fog color blended with the pixel
                                // color when bit 17 of the T3D_CMD_SET register
                                // is set to 1. This operation is also called
                                // depth cueing when the fog factor (source
                                // alpha) corresponds to the distance from the
                                // viewer
        ULONG T3D_COLOR0;       // reserverd, set by library
        ULONG T3D_COLOR1;       // reserved, set by library
        ULONG T3D_CMD_SET;      // 3D command register
        ULONG T3D_TBV;          // reserved, set by library
        ULONG T3D_TBU;          // reserved, set by library
        long  T3D_TdWdX;        // Triangle WX Delta (S12.19)
        long  T3D_TdWdY;        // Triangle WY Delta (S12.19)
        ULONG T3D_TWS;          // Triangle W Start
        long  T3D_TdDdX;        // Triangle DX Delta (S4.8.19)
        long  T3D_TdVdX;        // Triangle VX Delta (S12.8.11 / S24.7)
        long  T3D_TdUdX;        // Triangle UX Delta (S12.8.11 / S24.7)
        long  T3D_TdDdY;        // Triangle DY Delta (S4.8.19)
        long  T3D_TdVdY;        // Triangle VY Delta (S12.8.11 / S24.7)
        long  T3D_TdUdY;        // Triangle UY Delta (S12.8.11 / S24.7)
        ULONG T3D_TDS;          // Triangle D Start (S4.8.19)
        ULONG T3D_TVS;          // Triangle V Start (S12.8.11 / S24.7)
        ULONG T3D_TUS;          // Triangle U Start (S12.8.11 / S24.7)
        long  T3D_TdGdX_dBdX;   // Triangle GBX Delta (S8.7)
        long  T3D_TdAdX_dRdX;   // Triangle ARX Delta (S8.7)
        long  T3D_TdGdY_dBdY;   // Triangle GBY Delta (S8.7)
        long  T3D_TdAdY_dRdY;   // Triangle ARY Delta (S8.7)
        ULONG T3D_TGS_BS;       // Triangle GB Start (S8.7)
        ULONG T3D_TAS_RS;       // Triangle AR Start (S8.7)
        long  T3D_TdZdX;        // Triangle ZX Delta (S16.15)
        long  T3D_TdZdY;        // Triangle ZY Delta (S16.15)
        ULONG T3D_TZS;          // Triangle Z Start (S16.15)
        long  T3D_TdXdY12;      // Triangle XY12 (S11.20)
        ULONG T3D_TXEND12;      // Triangle X12 End (S11.20)
        long  T3D_TdXdY01;      // Triangle XY01 Delta (S11.20)
        ULONG T3D_TXEND01;      // Triangle X01 End (S11.20)
        long  T3D_TdXdY02;      // Triangle XY02 Delta (S11.20)
        ULONG T3D_TXS;          // Triangle X Start (S11.20)
        ULONG T3D_TYS;          // Triangle Y Start (S11.20)
        ULONG T3D_TY01_Y12;     // Triangle Y Count
} V3DTriangle;



//
// View3D tags
//

#define V3DVA_UseZBuffer        (0x8aa00000)    // zbuffer enable/disable
#define V3DVA_Colors32          (0x8aa00001)    // color palette array
#define V3DVA_UpdateZBuffer	(0x8aa00002)	// zbuffer update enable/disable
#define V3DVA_ZBufCmpMode	(0x8aa00003)	// zbuffer compare mode
#define V3DVA_ZBufInitVal	(0x8aa00004)	// zbuffer initial value
#define V3DVA_ClipRect3D        (0x8aa00005)    // clipping window for 3d operations (v3)
                                                // ti_Data points to a Rectangle32
#define V3DVA_Screen            (0x8aa0000a)    // only available for GetViewAttr()
                                                // private !!
//
// zbuffer compare modes
//
#define ZBFCMP_NEVERPASS	(0UL)		// Z compare never passes
#define ZBFCMP_zsGTzzb		(1UL)		// Z source larger than buffer Z
#define ZBFCMP_zsEQzzb		(2UL)		// Z source equals buffer Z
#define ZBFCMP_zsGEzzb		(3UL)		// Z source greater or equal
#define ZBFCMP_zsLTzzb		(4UL)		// Z source less than buffer Z
#define ZBFCMP_zsNEzzb		(5UL)		// Z source not equal buffer Z
#define ZBFCMP_zsLEzzb		(6UL)		// Z source less or equal
#define ZBFCMP_PASSALWAYS	(7UL)		// Z compare always passes 


//
// TexHandle tags
//
#define V3DTHA_MapSize           (0x8ab00000)   // mipmap/texmap level size
                                                //
                                                // Value = s, where 2**s is
                                                // the size of one side of the
                                                // largest mipmap texture
                                                // rectangle. For example, a
                                                // value of 4 specifies the
                                                // largest mipmap as 2**4 x
                                                // 2**4 = 16 x 16 texels. The
                                                // largest allowable s value
                                                // is 9, which specifies a
                                                // 512 x 512 texel texture.

#define V3DTHA_TexClrFmt         (0x8ab00001)   // texture color format
#define V3DTHA_FilterMode        (0x8ab00002)   // texture filtering mode
#define V3DTHA_TexWrap           (0x8ab00003)   // texture wrap on/off
#define V3DTHA_LitTexture        (0x8ab00004)   // texture is lit
#define V3DTHA_TexelMap          (0x8ab00005)   // pointer to texture map
#define V3DTHA_AlphaBlend        (0x8ab00006)   // alpha blend mode
#define V3DTHA_IsMipMap		 (0x8ab00007)   // texhandle uses texmap/mipmap
#define V3DTHA_Perspective       (0x8ab00008)   // perspective correction on/off
#define V3DTHA_TexBrdrClr        (0x8ab00009)   // tex border col if wrap off
#define V3DTHA_TexColor0         (0x8ab0000a)   // color0 for blend fmts
#define V3DTHA_TexColor1         (0x8ab0000b)   // color1 for blend fmts
#define V3DTHA_ErrorCode         (0x8ab0000c)   // ti_Data points to LONG
                                                // errorcode

//
// CreateTexHandle error codes
//

#define V3DTERR_OK              (0UL)   // all fine
#define V3DTERR_NOTEXMEM        (1UL)   // no more texture memory
#define V3DTERR_NOMAPSIZE       (2UL)   // no map size given (mandatory)
#define V3DTERR_NOTEXFMT        (3UL)   // no texture format given (mandatory)
#define V3DTERR_NOMEM           (4UL)   // no more system memory
#define V3DTERR_INVMAPSIZE      (5UL)   // map size not supported
#define V3DTERR_INVMAPFMT       (6UL)   // texel color format not supported
#define V3DTERR_INVFLTRMODE     (7UL)   // filter mode is invalid

//
// alpha blend modes
//
#define ABLEND_USESOURCE       (-1L)    // use source alpha for blending
#define ABLEND_NONE            (0UL)    // no alpha blending
#define ABLEND_USETEXTURE      (1UL)    // use texture alpha




//
// texel color formats
//
#define TEXFMT_ARGB8888         (0UL)   // 32 bits/pixel
#define TEXFMT_ARGB4444         (1UL)   // 16 bits/pixel
#define TEXFMT_ARGB1555         (2UL)   // 16 bits/pixel
#define TEXFMT_ALP4BLE4         (3UL)   // 8 bits/pixel (Alpha4, Blend4)
#define TEXFMT_BLEND4LO         (4UL)   // 4 bits/pixel (Blend4, low nibble)
#define TEXFMT_BLEND4HI         (5UL)   // 4 bits/pixel (Blend4, high nibble)
#define TEXFMT_LUT8             (6UL)   // 8 bits/pixel (palettized)
#define TEXFMT_YUYV16           (7UL)   // YU/YV 16 bits/pixel equivalent

//
// texture filtering modes
// Only modes with no filtering (1TPP) can be used with 8 bits/pixel palettized
// data. In addition, the texture blending mode must be decal.
//

#define FLTRMD_M1TPP		(0UL)	// mipmap nearest
#define FLTRMD_M2TPP		(1UL)	// interpolate texel between mipmaps
#define FLTRMD_M4TPP		(2UL)	// interpolate tex 1,2,3,4 mmap nearest
#define FLTRMD_M8TPP		(3UL)	// interp. texel 1,2,3,4 between mmaps
#define FLTRMD_1TPP             (4UL)   // no filtering
#define FLTRMD_V2TPP            (5UL)   // used for YU/UV video format
#define FLTRMD_4TPP             (6UL)   // interpolate texel 1,2,3,4



//
// texture blending modes
//
#define BLENDMD_COMPREF         (0UL)   // complex reflection
                                        // adds the (normalized, 0 = black and
                                        // 1 = white) texel and pixel colors,
                                        // with a maximum value of 1. This
                                        // lightens the pixel.
                                        //
#define BLENDMD_MODULATE        (1UL)   // modulate
                                        // multiplies the normalized color
                                        // values. This results in a smaller
                                        // value (darker pixel). The programmer
                                        // may need to compensate for this
                                        // darkening effect.
                                        //
#define BLENDMD_DECAL           (2UL)   // decal
                                        // replaces the source pixel color
                                        // with the textel color, essentially
                                        // overlaying the teture on the scene.
                                        // this is the only mode that can be
                                        // used with palettized data

#define BLENDMD_SRCALPHA        (4UL)   // use source alpha value for blending
                                        // This mode is used in conjunction with
                                        // the other 3 blending modes. OR it
                                        // with the actual blending mode to
                                        // incorporate source pixel alpha value
                                        // into blending.

//
// rectangle formats for V3D_WriteRect()
//

#define RECT2D_LUT8		(0UL)	// 2d rectangle 1byte/pixel palettized
#define RECT2D_RGB		(1UL)	// 2d rectangle 3bytes/pixel truecolour
#define RECT2D_ARGB		(2UL)	// 2d rectangle 4bytes/pixel truecolour
#define RECT2D_ZBUFFER		(3UL)	// 2d rect 2bytes/pixel zbuf data (v3)

//
// vertex list types for V3D_DrawTriangle3DSet()
//

#define T3DSET_TRILIST          (0UL)   // n/3 seperate triangles
                                        //
#define T3DSET_TRISTRIP         (1UL)   // n-2 connected triangles
                                        //
#define T3DSET_TRIFAN           (2UL)   // n-2 connected triangles having
                                        // first vertex in common
#endif
