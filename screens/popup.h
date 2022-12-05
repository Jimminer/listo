#ifndef _POPUP_H
#define _POPUP_H

#include "../utils.h"

enum POPUP{
    CREATE_POPUP,
    DELETE_POPUP,
    EDIT_POPUP
};

int initPopup(WINDOW *window, ListoInfo *listoInfo, int type);
void createPopup(WINDOW *window, ListoInfo *listoInfo, char *name);
int deletePopup(WINDOW *window, ListoInfo *listoInfo, char *name);

#endif