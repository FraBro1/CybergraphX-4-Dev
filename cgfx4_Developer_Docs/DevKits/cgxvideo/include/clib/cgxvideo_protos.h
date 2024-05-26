#ifndef CLIB_CGXVIDEO_H
#define CLIB_CGXVIDEO_H

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

struct VLayerHandle *CreateVLayerHandleTagList(struct Screen *,
                                                        struct TagItem *);

ULONG DeleteVLayerHandle(struct VLayerHandle *);

ULONG AttachVLayerTagList(struct VLayerHandle *,
                        struct Window *,
                        struct TagItem *);

ULONG DetachVLayer(struct VLayerHandle *);

ULONG GetVLayerAttr(struct VLayerHandle *,
                        ULONG AttrID);

ULONG UnLockVLayer(struct VLayerHandle *);

ULONG LockVLayer(struct VLayerHandle *);

void SetVLayerAttrTagList(struct VLayerHandle *,
				struct TagItem *);


ULONG AttachVLayerTags(struct VLayerHandle *,
                        struct Window *,
                        Tag, ...);

struct VLayerHandle *CreateVLayerHandleTags(struct Screen *,
                                                        Tag, ...);

void SetVLayerAttrTags(struct VLayerHandle *,
			Tag, ...);

#endif
