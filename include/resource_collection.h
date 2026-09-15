#ifndef MPR_RESOURCE_MANAGER_H
#define MPR_RESOURCE_MANAGER_H

#include <nitro/types.h>
#include <nnsys.h>

#include "constants/heap.h"

// In TEX_RESOURCE_MODE_SEPARATED, texture data is stripped from the resource and stored in a separate buffer.
// The main purpose of this is to allow freeing the texture data after the texture has been uploaded to VRAM,
// while still keeping the resource itself around.
typedef enum {
	TEX_RESOURCE_MODE_NORMAL = 0,
	TEX_RESOURCE_MODE_SEPARATED
} TextureResourceMode;

typedef struct Resource {
	int    id;
	void*  data;
} Resource;

typedef struct ResourceCollection {
	Resource*  resources;
	int        capacity;
	int        count;
} ResourceCollection;

typedef struct TextureResource {
	Resource*      resource;
	NNSGfdTexKey   texKey;
	NNSGfdTexKey   tex4x4Key;
	NNSGfdPlttKey  paletteKey;
	void*          textureData;       // Only used when mode is TEX_RESOURCE_MODE_STRIPPED
	u16            texDataDiscarded;
	u16            mode;
} TextureResource;

typedef struct {
	ResourceCollection*  resources;
	TextureResource*     textures;
} TextureResourceManager;

ResourceCollection* ResourceCollection_New(s32 capacity, HeapID heapID);
void ResourceCollection_Delete(ResourceCollection* collection);
BOOL ResourceCollection_IsIDUnused(ResourceCollection* collection, int id);
Resource* ResourceCollection_Add(ResourceCollection* collection, void* data, int id);
Resource* ResourceCollection_AddFromFile(ResourceCollection* collection, const char* filename, int id, HeapID heapID);
void ResourceCollection_Remove(ResourceCollection* collection, Resource* resource);
void ResourceCollection_Clear(ResourceCollection* collection);
Resource* ResourceCollection_FindResource(ResourceCollection* collection, int id);
void* Resource_GetData(Resource* resource);
int Resource_GetID(Resource* resource);

#endif /* MPR_RESOURCE_MANAGER_H */
