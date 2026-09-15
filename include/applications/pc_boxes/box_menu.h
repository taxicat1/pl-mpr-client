#ifndef MPR_APPLICATIONS_PC_BOXES_BOX_MENU_H
#define MPR_APPLICATIONS_PC_BOXES_BOX_MENU_H

#include "applications/pc_boxes/box_def.h"

void BoxMenu_FillYesNo(BoxApplication* boxApp, u32 menuItemIndex);
void BoxMenu_FillTopLevelMenuItems(BoxApplication* boxApp);
void BoxMenu_FillItemsMenu(BoxApplication* boxApp);
void BoxMenu_FillHeaderMenu(BoxApplication* boxApp);
void BoxMenu_FillRanchHeaderMenu(BoxApplication* boxApp, BOOL hasMoreThanTenBoxes);
void BoxMenu_FillWallpaperMenu(BoxApplication* boxApp, BoxMenuItem menuItem);
void BoxMenu_FillWallpaperSelectionMenu(BoxApplication* boxApp, BoxMenuItem menuItem);
void BoxMenu_FillMarkingsMenu(BoxApplication* boxApp);
void BoxMenu_ToggleMarking(BoxApplication* boxApp, u32 marking);
BoxMenuItem BoxMenu_GetMenuNavigation(BoxApplication* boxApp);
BoxMenuItem BoxMenu_GetSelectedMenuItem(BoxApplication* boxApp);
BoxMenuItem BoxMenu_GetDefaultMenuItem(BoxApplication* boxApp);

#endif /* MPR_APPLICATIONS_PC_BOXES_BOX_MENU_H */
