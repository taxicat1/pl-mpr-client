#include <nitro.h>
#include <string.h>

#include "sprite_transfer.h"

#include "assert.h"
#include "char_transfer.h"
#include "pltt_transfer.h"
#include "sprite_resource.h"


BOOL SpriteTransfer_RequestChar(const SpriteResource* resource) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_CHAR);
	
	CharTransferTaskTemplate template;
	template.data = SpriteResource_GetTileData(resource);
	template.vramType = SpriteResource_GetVRAMType(resource);
	template.resourceID = SpriteResource_GetID(resource);
	template.atEnd = FALSE;
	return CharTransfer_Request(&template);
}


void SpriteTransfer_RequestCharList(const SpriteResourceList* resourceList) {
	GF_ASSERT(resourceList);
	for (int i = 0; i < resourceList->capacity; i++) {
		if (resourceList->resources[i]) {
			GF_ASSERT(SpriteTransfer_RequestChar(resourceList->resources[i]));
		}
	}
}


void SpriteTransfer_RetransferCharData(const SpriteResource* resource) {
	GF_ASSERT(resource);
	CharTransfer_ReplaceCharData(SpriteResource_GetID(resource), SpriteResource_GetTileData(resource));
}


void SpriteTransfer_ResetCharTransfer(SpriteResource* resource) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_CHAR);
	CharTransfer_ResetTask(SpriteResource_GetID(resource));
}


void SpriteTransfer_ResetCharTransferList(SpriteResourceList* resourceList) {
	GF_ASSERT(resourceList);
	for (int i = 0; i < resourceList->capacity; i++) {
		if (resourceList->resources[i]) {
			SpriteTransfer_ResetCharTransfer(resourceList->resources[i]);
		}
	}
}


NNSG2dImageProxy* SpriteTransfer_GetImageProxy(const SpriteResource* resource) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_CHAR);
	return CharTransfer_GetImageProxy(SpriteResource_GetID(resource));
}


NNSG2dImageProxy* SpriteTransfer_GetCellTransferProxy(const SpriteResource* charResource, const SpriteResource* cellResource) {
	GF_ASSERT(charResource);
	GF_ASSERT(SpriteResource_GetType(charResource) == SPRITE_RESOURCE_CHAR);
	GF_ASSERT(cellResource);
	GF_ASSERT(SpriteResource_GetType(cellResource) == SPRITE_RESOURCE_CELL);
	
	int resourceID = SpriteResource_GetID(charResource);
	NNSG2dCellDataBank* cellData = SpriteResource_GetSpriteData(cellResource);
	NNSG2dImageProxy* imageProxy = CharTransfer_ResizeTaskRange(resourceID, cellData->pVramTransferData->szByteMax);
	
	if (imageProxy == NULL) {
		imageProxy = CharTransfer_CopyTask(CharTransfer_GetImageProxy(resourceID));
	}
	
	return imageProxy;
}


BOOL SpriteTransfer_RequestPlttWholeRange(const SpriteResource* resource) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_PLTT);
	
	PlttTransferTaskTemplate template;
	template.data = SpriteResource_GetPaletteFade(resource);
	template.vramType = SpriteResource_GetVRAMType(resource);
	template.resourceID = SpriteResource_GetID(resource);
	template.plttIndex = SpriteResource_GetPaletteIndex(resource);
	return PlttTransfer_RequestWholeRange(&template);
}


void SpriteTransfer_RequestPlttWholeRangeList(const SpriteResourceList* resourceList) {
	GF_ASSERT(resourceList);
	for (int i = 0; i < resourceList->capacity; i++) {
		if (resourceList->resources[i]) {
			GF_ASSERT(SpriteTransfer_RequestPlttWholeRange(resourceList->resources[i]));
		}
	}
}


void SpriteTransfer_ReplacePlttData(const SpriteResource* resource) {
	GF_ASSERT(resource);
	PlttTransfer_ReplacePlttData(SpriteResource_GetID(resource), SpriteResource_GetPaletteFade(resource));
}


void SpriteTransfer_ResetPlttTransfer(SpriteResource* resource) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_PLTT);
	PlttTransfer_ResetTask(SpriteResource_GetID(resource));
}


void SpriteTransfer_ResetPlttTransferList(SpriteResourceList* resourceList) {
	GF_ASSERT(resourceList);
	for (int i = 0; i < resourceList->capacity; i++) {
		if (resourceList->resources[i]) {
			SpriteTransfer_ResetPlttTransfer(resourceList->resources[i]);
		}
	}
}


const NNSG2dImagePaletteProxy* SpriteTransfer_GetPaletteProxy(const SpriteResource* resource, NNSG2dImageProxy* imageProxy) {
	GF_ASSERT(resource);
	GF_ASSERT(SpriteResource_GetType(resource) == SPRITE_RESOURCE_PLTT);
	
	int resourceID = SpriteResource_GetID(resource);
	if (imageProxy) {
		return PlttTransfer_ToggleExtPalette(resourceID, imageProxy);
	}
	
	return PlttTransfer_GetPaletteProxy(resourceID);
}
