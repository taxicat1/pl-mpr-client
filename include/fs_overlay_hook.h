#ifndef MPR_FS_OVERLAY_HOOK
#define MPR_FS_OVERLAY_HOOK

// Ignore overlays when linking, emit 0xFFFFFFFF as the overlay ID instead

#ifdef FS_EXTERN_OVERLAY
#undef FS_EXTERN_OVERLAY
#endif

#define FS_EXTERN_OVERLAY(x)   extern u8 SDK_OVERLAY_NUMBER[]

#ifdef FS_OVERLAY_ID
#undef FS_OVERLAY_ID
#endif

#define FS_OVERLAY_ID(x)       (u32)(&SDK_OVERLAY_NUMBER[-1])

#endif /* MPR_FS_OVERLAY_HOOK */
