#include <nitro.h>
#include <string.h>

#include "resource_collection.h"

#include "assert.h"
#include "constants/heap.h"
#include "heap.h"
#include "system.h"

#define RESOURCE_ID_INVALID  -1

static Resource* ResourceCollection_AllocResource(ResourceCollection* collection);
static void Resource_Init(Resource* resource);


ResourceCollection* ResourceCollection_New(s32 capacity, HeapID heapID) {
	ResourceCollection* resMgr = Heap_Alloc(heapID, sizeof(ResourceCollection));
	GF_ASSERT(resMgr);
	
	resMgr->resources = Heap_Alloc(heapID, sizeof(Resource) * capacity);
	GF_ASSERT(resMgr->resources);
	
	for (int i = 0; i < capacity; i++) {
		Resource_Init(resMgr->resources + i);
	}
	
	resMgr->capacity = capacity;
	resMgr->count = 0;
	
	return resMgr;
}


void ResourceCollection_Delete(ResourceCollection* collection) {
	GF_ASSERT(collection);
	
	ResourceCollection_Clear(collection);
	Heap_Free(collection->resources);
	Heap_Free(collection);
}


BOOL ResourceCollection_IsIDUnused(ResourceCollection* collection, int id) {
	GF_ASSERT(collection);
	return ResourceCollection_FindResource(collection, id) == NULL;
}


Resource* ResourceCollection_Add(ResourceCollection* collection, void* data, int id) {
	GF_ASSERT(collection);
	
	Resource* resource = ResourceCollection_AllocResource(collection);
	GF_ASSERT(resource);
	
	GF_ASSERT(ResourceCollection_IsIDUnused(collection, id) == TRUE);
	
	resource->data = data;
	resource->id = id;
	collection->count++;
	
	return resource;
}


Resource* ResourceCollection_AddFromFile(ResourceCollection* collection, const char* filename, int id, HeapID heapID) {
	GF_ASSERT(collection);
	GF_ASSERT(filename);
	
	Resource* resource = ResourceCollection_AllocResource(collection);
	
	GF_ASSERT(resource);
	GF_ASSERT(ResourceCollection_IsIDUnused(collection, id) == TRUE);
	
	resource->data = ReadFileToHeap(heapID, filename);
	GF_ASSERT(resource->data);
	resource->id = id;
	
	collection->count++;
	
	return resource;
}


void ResourceCollection_Remove(ResourceCollection* collection, Resource* resource) {
	GF_ASSERT(collection);
	GF_ASSERT(resource);
	
	if (resource->data) {
		Heap_Free(resource->data);
		resource->data = NULL;
	}
	
	resource->id = RESOURCE_ID_INVALID;
	collection->count--;
}


void ResourceCollection_Clear(ResourceCollection* collection) {
	GF_ASSERT(collection);
	GF_ASSERT(collection->resources);
	
	for (int i = 0; i < collection->capacity; i++) {
		if (collection->resources[i].id != RESOURCE_ID_INVALID) {
			ResourceCollection_Remove(collection, collection->resources + i);
		}
	}
}


Resource* ResourceCollection_FindResource(ResourceCollection* collection, int id) {
	GF_ASSERT(collection);
	
	for (int i = 0; i < collection->capacity; i++) {
		if (collection->resources[i].id == id) {
			return collection->resources + i;
		}
	}
	
	return NULL;
}


void* Resource_GetData(Resource* resource) {
	GF_ASSERT(resource);
	return resource->data;
}


int Resource_GetID(Resource* resource) {
	GF_ASSERT(resource);
	return resource->id;
}


static Resource* ResourceCollection_AllocResource(ResourceCollection* collection) {
	GF_ASSERT(collection);
	
	for (int i = 0; i < collection->capacity; i++) {
		if (collection->resources[i].id == RESOURCE_ID_INVALID) {
			return collection->resources + i;
		}
	}
	
	return NULL;
}


static void Resource_Init(Resource* resource) {
	GF_ASSERT(resource);
	
	resource->id = RESOURCE_ID_INVALID;
	resource->data = NULL;
}
