#ifndef MPR_BOOT_H
#define MPR_BOOT_H

void AccessOuterRomFS(void);
void RebootAndLoadROM(const char* filesystemPath);

#endif /* MPR_BOOT_H */