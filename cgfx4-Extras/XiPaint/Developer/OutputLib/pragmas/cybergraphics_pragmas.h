#ifndef CyberGfxBase_PRAGMA_H
#define CyberGfxBase_PRAGMA_H

#pragma libcall CyberGfxBase IsCyberModeID 36 001
#pragma libcall CyberGfxBase BestCModeIDTagList 3c 801
#pragma libcall CyberGfxBase CModeRequestTagList 42 9802
#pragma libcall CyberGfxBase AllocCModeListTagList 48 901
#pragma libcall CyberGfxBase FreeCModeList 4e 801
#pragma libcall CyberGfxBase ScalePixelArray 5a 76543921080a
#pragma libcall CyberGfxBase GetCyberMapAttr 60 0802
#pragma libcall CyberGfxBase GetCyberIDAttr 66 1002
#pragma libcall CyberGfxBase ReadRGBPixel 6c 10903
#pragma libcall CyberGfxBase WriteRGBPixel 72 210904
#pragma libcall CyberGfxBase ReadPixelArray 78 76543921080a
#pragma libcall CyberGfxBase WritePixelArray 7e 76543921080a
#pragma libcall CyberGfxBase MovePixelArray 84 543291007
#pragma libcall CyberGfxBase SwapPixelArray 8a 76543921080a
#pragma libcall CyberGfxBase InvertPixelArray 90 3210905
#pragma libcall CyberGfxBase FillPixelArray 96 43210906
#pragma libcall CyberGfxBase DoCDrawMethodTagList 9c a9803
#pragma libcall CyberGfxBase VideoCtrlTagList a2 9802

#ifdef __SASC_60
#pragma tagcall CyberGfxBase BestCModeIDTags 3c 801
#pragma tagcall CyberGfxBase CModeRequestTags 42 9802
#pragma tagcall CyberGfxBase AllocCModeListTags 48 901
#pragma tagcall CyberGfxBase DoCDrawMethodTags 9c a9803
#pragma tagcall CyberGfxBase VideoCtrlTags a2 9802
#endif
#endif
