#include "popup.h"

void createPopup(WINDOW *window, ListoInfo *listoInfo, char *name){
    int input;
    int index = listoInfo->scrollOffset + listoInfo->selectedLine;
    int selection = 0;
    int entered = 0;
    int typedChars = 0;

    werase(window);
    box(window, 0, 0);
    mvwprintw(window, 0, 2, name);

    strcpy(listoInfo->taskInfo->title, "");
    listoInfo->taskInfo->index = index;
    listoInfo->taskInfo->placement = 0;
    listoInfo->taskInfo->property = 0;
    listoInfo->taskInfo->bullet = 0;
    listoInfo->taskInfo->valid = 0;

    while (input = getch()){
        switch (input){
            case 'q':
                curs_set(0);
                return;
                break;

            case KEY_BACKSPACE:
                if (selection == 0 && entered && typedChars > 0){
                    mvwprintw(window, 2, 8 + typedChars, " ");
                    typedChars--;
                    listoInfo->taskInfo->title[typedChars] = '\0';
                }
                break;

            case 10:
                entered = !entered;
                break;

            case KEY_DOWN:
                if (selection < 4 && !entered)
                    selection++;
                break;

            case KEY_UP:
                if (selection > 0 && !entered)
                    selection--;
                break;
            
            default:
                break;
        }



        if (selection == 0 && !entered) wattron(window, A_REVERSE);
        mvwprintw(window, 2, 2, "Title");
        if (selection == 0 && !entered) wattroff(window, A_REVERSE);

        wprintw(window, ": ");

        if (listoInfo->taskInfo->property == 1)
            wattron(window, COLOR_PAIR(COLOR_IMPORTANT) | ATTR_IMPORTANT);
        else if (listoInfo->taskInfo->property == 2)
            wattron(window, COLOR_PAIR(COLOR_QUESTIONABLE) | ATTR_QUESTIONABLE);
        else if (listoInfo->taskInfo->property == 3)
            wattron(window, COLOR_PAIR(COLOR_DONE) | ATTR_DONE);
        wprintw(window, "%s", listoInfo->taskInfo->title);
        wattroff(window, COLOR_PAIR(COLOR_IMPORTANT) | COLOR_PAIR(COLOR_QUESTIONABLE) | COLOR_PAIR(COLOR_DONE) | ATTR_IMPORTANT | ATTR_QUESTIONABLE | ATTR_DONE);



        if (selection == 1 && !entered) wattron(window, A_REVERSE);
        mvwprintw(window, 3, 2, "Property");
        if (selection == 1 && !entered) wattroff(window, A_REVERSE);

        mvwprintw(window, 3, 10, ":             ");

        if (selection == 1 && entered) wattron(window, A_REVERSE);
        if (listoInfo->taskInfo->property == 0)
            mvwprintw(window, 3, 12, "None");
        else if (listoInfo->taskInfo->property == 1)
            mvwprintw(window, 3, 12, "Important");
        else if (listoInfo->taskInfo->property == 2)
            mvwprintw(window, 3, 12, "Questionable");
        else if (listoInfo->taskInfo->property == 3)
            mvwprintw(window, 3, 12, "Done");
        if (selection == 1 && entered) wattroff(window, A_REVERSE);




        if (selection == 2 && !entered) wattron(window, A_REVERSE);
        mvwprintw(window, 4, 2, "Bullet");
        if (selection == 2 && !entered) wattroff(window, A_REVERSE);

        mvwprintw(window, 4, 8, ":    ");

        if (selection == 2 && entered) wattron(window, A_REVERSE);
        if (listoInfo->taskInfo->bullet == 0)
            mvwprintw(window, 4, 10, "No");
        else if (listoInfo->taskInfo->bullet == 1)
            mvwprintw(window, 4, 10, "Yes");
        if (selection == 2 && entered) wattroff(window, A_REVERSE);



        if (selection == 3 && !entered) wattron(window, A_REVERSE);
        mvwprintw(window, 5, 2, "Placement");
        if (selection == 3 && !entered) wattroff(window, A_REVERSE);

        mvwprintw(window, 5, 11, ":      ");

        if (selection == 3 && entered) wattron(window, A_REVERSE);
        if (listoInfo->taskInfo->placement == 0)
            mvwprintw(window, 5, 13, "Above");
        else if (listoInfo->taskInfo->placement == 1)
            mvwprintw(window, 5, 13, "Below");
        if (selection == 3 && entered) wattroff(window, A_REVERSE);



        if (selection == 4) wattron(window, A_REVERSE);
        mvwprintw(window, 7, 2, "Create");
        if (selection == 4) wattroff(window, A_REVERSE);





        if (selection == 0 && entered){
            curs_set(1);
            wmove(window, 2, 9+typedChars);
            if (input >= ' ' && input <= '~' && (9+typedChars) < window->_maxx - 2){
                listoInfo->taskInfo->title[typedChars] = input;
                listoInfo->taskInfo->title[typedChars+1] = '\0';
                typedChars++;
            }
        }
        else {
            curs_set(0);
        }



        if (selection == 1 && entered){
            if (input == KEY_LEFT)
                listoInfo->taskInfo->property--;
            else if (input == KEY_RIGHT)
                listoInfo->taskInfo->property++;

            if (listoInfo->taskInfo->property < 0) listoInfo->taskInfo->property = 0;
            else if (listoInfo->taskInfo->property > 3) listoInfo->taskInfo->property = 3;
        }



        if (selection == 2 && entered){
            if (input == KEY_LEFT)
                listoInfo->taskInfo->bullet = 0;
            else if (input == KEY_RIGHT)
                listoInfo->taskInfo->bullet = 1;
        }



        if (selection == 3 && entered){
            if (input == KEY_LEFT)
                listoInfo->taskInfo->placement = 0;
            else if (input == KEY_RIGHT)
                listoInfo->taskInfo->placement = 1;
        }



        if (selection == 4 && entered){
            if (typedChars > 0){

                strcpy(listoInfo->tempBuffer, "");
                
                if (listoInfo->taskInfo->property == 1)
                    listoInfo->tempBuffer[0] = '!';
                else if (listoInfo->taskInfo->property == 2)
                    listoInfo->tempBuffer[0] = '?';
                else if (listoInfo->taskInfo->property == 3)
                    listoInfo->tempBuffer[0] = '~';
                
                if (!listoInfo->taskInfo->bullet && listoInfo->taskInfo->property != 0)
                    listoInfo->tempBuffer[1] = '\0';
                else if (listoInfo->taskInfo->bullet && listoInfo->taskInfo->property == 0){
                    listoInfo->tempBuffer[0] = '-';
                    listoInfo->tempBuffer[1] = '\0';
                }
                else if (listoInfo->taskInfo->bullet && listoInfo->taskInfo->property != 0){
                    listoInfo->tempBuffer[1] = listoInfo->tempBuffer[0];
                    listoInfo->tempBuffer[0] = '-';
                    listoInfo->tempBuffer[2] = '\0';
                }
                
                strcat(listoInfo->tempBuffer, listoInfo->taskInfo->title);
                strcpy(listoInfo->taskInfo->title, listoInfo->tempBuffer);
                
                listoInfo->taskInfo->valid = 1;
            }
            curs_set(0);
            return;
        }

        wrefresh(window);
    }
}