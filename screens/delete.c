#include "popup.h"

int deletePopup(WINDOW *window, ListoInfo *listoInfo, char *name){
    int input;
    int selection = 0;

    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 2, name);

    while (input = getch()){
        switch (input){
            case 'q':
                return 0;
                break;

            case 10:
                return selection;
                break;

            case KEY_LEFT:
                selection = 0;
                break;

            case KEY_RIGHT:
                selection = 1;
                break;
            
            default:
                break;
        }

        mvwprintw(window, 1, window->_maxx/2 - 25, "Are you sure you want to delete the selected task?");

        if (selection == 0) wattron(window, A_REVERSE);
        mvwprintw(window, 3, window->_maxx/4, "No");
        if (selection == 0) wattroff(window, A_REVERSE);

        if (selection == 1) wattron(window, A_REVERSE);
        mvwprintw(window, 3, window->_maxx/4*3, "Yes");
        if (selection == 1) wattroff(window, A_REVERSE);

        wrefresh(window);
    }
}