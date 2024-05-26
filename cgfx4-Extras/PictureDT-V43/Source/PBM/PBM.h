
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <graphics/gfxbase.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/cghooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuitionbase.h>
#include <dos/dosextens.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include <datatypes/pictureclassext.h>
#include <cybergraphics/cybergraphics.h>

#define __USE_SYSBASE 42

#include <clib/alib_protos.h>
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#define REG(r) register __ ## r

struct PBMBase
 {
  struct Library PB_Lib;
  BPTR PB_SegList;
  Class *PB_Class;
  struct SignalSemaphore PB_Semaphore;
 };

#define PBMDTCLASS PBMBase->PB_Lib.lib_Node.ln_Name

LONG __regargs ReadPBM(Class *,Object *,struct TagItem *);

void __regargs CloseFFR(struct FFRHandle *);
struct FFRHandle __regargs *OpenFFR(BPTR);

LONG __regargs FFRGetC(struct FFRHandle *);
LONG __regargs FFRRead(struct FFRHandle *,APTR,LONG);

#define READ(h,p,b) (FFRRead(h,p,b)!=(b))
#define MySetIoErr(e) ((struct Process *)FindTask(NULL))->pr_Result2=(e)
