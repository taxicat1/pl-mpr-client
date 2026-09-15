#ifndef MPR_FS_LOCAL
#define MPR_FS_LOCAL

#define LOCAL_FILE(file)      FSLocal_ ## file
#define LOCAL_FILE_END(file)  FSLocal_ ## file ## _END

#define LOCAL_FILE_SIZE(file)  ((u32)LOCAL_FILE_END(file) - (u32)LOCAL_FILE(file))

// Note declaring const is required to match, despite the objects being linked to .data
#define LOCAL_FILE_DECL(file)  extern const u8 LOCAL_FILE(file)[], LOCAL_FILE_END(file)[]

LOCAL_FILE_DECL(mpr_string_table_bin);
LOCAL_FILE_DECL(ds_back_ncg);
LOCAL_FILE_DECL(ds_back_ncl);
LOCAL_FILE_DECL(ds_back_1_nsc);
LOCAL_FILE_DECL(ds_back_2_nsc);
LOCAL_FILE_DECL(NARC_box_m_obj_ncg);
LOCAL_FILE_DECL(warning_bg_ncg);
LOCAL_FILE_DECL(warning_bg_ncl);
LOCAL_FILE_DECL(box_wallpaper_ncer);
LOCAL_FILE_DECL(box_wallpaper_nanr);
LOCAL_FILE_DECL(box_wallpaper_ncgr_LZ);
LOCAL_FILE_DECL(box_wallpaper_nclr);
LOCAL_FILE_DECL(box_wallpaper_nscr);

#endif /* MPR_FS_LOCAL */
