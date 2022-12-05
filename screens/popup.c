#include "popup.h"

int initPopup(WINDOW *window, ListoInfo *listoInfo, int type){
    switch (type){
        case CREATE_POPUP:
            createPopup(window, listoInfo, " Create new task ");
            break;
        
        case DELETE_POPUP:
            return deletePopup(window, listoInfo, " Delete selected task ");
            break;
        
        default:
            break;
    }
}