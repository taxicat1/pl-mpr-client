#ifndef MPR_SPRITE_TRANSFER_H
#define MPR_SPRITE_TRANSFER_H

#include <nnsys.h>

#include "sprite_resource.h"

BOOL SpriteTransfer_RequestChar(const SpriteResource* resource);
void SpriteTransfer_RequestCharList(const SpriteResourceList* resourceList);
void SpriteTransfer_RetransferCharData(const SpriteResource* resource);
void SpriteTransfer_ResetCharTransfer(SpriteResource* resource);
void SpriteTransfer_ResetCharTransferList(SpriteResourceList* resourceList);
NNSG2dImageProxy* SpriteTransfer_GetImageProxy(const SpriteResource* resource);
NNSG2dImageProxy* SpriteTransfer_GetCellTransferProxy(const SpriteResource* charResource, const SpriteResource* cellResource);
BOOL SpriteTransfer_RequestPlttWholeRange(const SpriteResource* resource);
void SpriteTransfer_RequestPlttWholeRangeList(const SpriteResourceList* resourceList);
void SpriteTransfer_ReplacePlttData(const SpriteResource* resource);
void SpriteTransfer_ResetPlttTransfer(SpriteResource* resource);
void SpriteTransfer_ResetPlttTransferList(SpriteResourceList* resourceList);
const NNSG2dImagePaletteProxy* SpriteTransfer_GetPaletteProxy(const SpriteResource* resource, NNSG2dImageProxy* imageProxy);

#endif /* MPR_SPRITE_TRANSFER_H */
