#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// #define NCURSES_WIDECHAR 1

#include "utils.h"
#include "screens/popup.h"

// TODO: split utils into more modules
// TODO: implement edit popup
// TODO: make a menu creator module (?)
// TODO: adapt to terminal size change
// TODO: clean-up the codebase

int main(int argc, char **argv){
    setlocale(LC_ALL, ""); // Output Unicode correctly (ex. a bullet point)
    initscr();

{
        // Custom Borders

        // cchar_t *BORDER_LEFT, *BORDER_RIGHT, *BORDER_TOP, *BORDER_BOTTOM, *BORDER_TOP_LEFT, *BORDER_TOP_RIGHT, *BORDER_BOTTOM_LEFT, *BORDER_BOTTOM_RIGHT;
        // setcchar(BORDER_LEFT, (wchar_t*)"│", 0, 0, NULL);
        // setcchar(BORDER_RIGHT, (wchar_t*)"│", 0, 0, NULL);
        // setcchar(BORDER_TOP, (wchar_t*)"─", 0, 0, NULL);
        // setcchar(BORDER_BOTTOM, (wchar_t*)"─", 0, 0, NULL);
        // setcchar(BORDER_TOP_LEFT, (wchar_t*)"┌", 0, 0, NULL);
        // setcchar(BORDER_TOP_RIGHT, (wchar_t*)"┐", 0, 0, NULL);
        // setcchar(BORDER_BOTTOM_LEFT, (wchar_t*)"└", 0, 0, NULL);
        // setcchar(BORDER_BOTTOM_RIGHT, (wchar_t*)"┘", 0, 0, NULL);
        // wborder_set(win1, BORDER_LEFT, BORDER_RIGHT, BORDER_TOP, BORDER_BOTTOM, BORDER_TOP_LEFT, BORDER_TOP_RIGHT, BORDER_BOTTOM_LEFT, BORDER_BOTTOM_RIGHT);
        // wborder_set(win2, BORDER_LEFT, BORDER_RIGHT, BORDER_TOP, BORDER_BOTTOM, BORDER_TOP_LEFT, BORDER_TOP_RIGHT, BORDER_BOTTOM_LEFT, BORDER_BOTTOM_RIGHT);
}

        noecho();
        curs_set(0);
        keypad(stdscr, true);
        timeout(0);

        int input, currentScreen;
        int yMax, xMax;
        
        int timer = 0;
        int inputTimer = 0;

        ListoInfo listoInfo;
        listoInfo.scrollOffset = 0;
        listoInfo.selectedLine = 0;
        listoInfo.todoListSize = 0;
        listoInfo.todoListNotPrinted = 0;

        getmaxyx(stdscr, yMax, xMax);
        if (yMax < MINIMUM_Y || xMax < MINIMUM_X){
            cleanup(&listoInfo);
            printf("Terminal is too small! Current Size: (%ix%i) | Minimum size: (%ix%i)\n", xMax, yMax, MINIMUM_X, MINIMUM_Y);
            exit(0);
        }

        findSaveLocation(&listoInfo.saveLocation);
        listoInfo.buffer = (char *)malloc(TEXT_BUFFER_SIZE);
        listoInfo.tempBuffer = (char *)malloc(TEXT_BUFFER_SIZE);
        listoInfo.taskInfo = (TaskInfo *)malloc(sizeof(TaskInfo));
        listoInfo.taskInfo->title = (char *)malloc(TEXT_BUFFER_SIZE);

        WINDOW *window = newwin(yMax-2, xMax-2, 1, 1);
        WINDOW *popup = newwin(yMax/4, xMax/1.5, yMax/2 - yMax/8, xMax/2 - xMax/3);

        box(stdscr, 0, 0);

        initColors(&listoInfo);
        
        if (argc > 1) currentScreen = atoi(argv[1]);
        else currentScreen = TODO_SCREEN;

        wrefresh(window);
        switchScreen(window, currentScreen);
        refresh();
        getTodoListInfo(&listoInfo);
        printTodoList(window, &listoInfo);


        while((input = getch()) != 'q'){
            napms(1);
            timer++;

            if (inputTimer >= timer/INPUT_INTERVAL || input == ERR) continue;

            inputTimer = timer/INPUT_INTERVAL;
            
            switch(input){
                case 'n':
                case 'N':
                    initPopup(popup, &listoInfo, CREATE_POPUP);
                    if (listoInfo.taskInfo->valid)
                        writeTodoList(&listoInfo, &listoInfo.taskInfo->title, listoInfo.taskInfo->index+listoInfo.taskInfo->placement);

                    getTodoListInfo(&listoInfo);

                    selectionCheck(&listoInfo, yMax);

                    werase(window);
                    printTodoList(window, &listoInfo);
                    break;

                case 330: // Delete
                    if (initPopup(popup, &listoInfo, DELETE_POPUP))
                        deleteTodoList(&listoInfo, listoInfo.selectedLine + listoInfo.scrollOffset);
                    
                    getTodoListInfo(&listoInfo);

                    selectionCheck(&listoInfo, yMax);

                    werase(window);
                    printTodoList(window, &listoInfo);
                    break;

                case KEY_DOWN:
                    if (listoInfo.selectedLine < yMax/2 && listoInfo.selectedLine < listoInfo.todoListSize-1)
                        listoInfo.selectedLine++;
                    else if ((listoInfo.scrollOffset + listoInfo.selectedLine) < listoInfo.todoListSize-1)
                        listoInfo.scrollOffset++;

                    getTodoListInfo(&listoInfo);

                    selectionCheck(&listoInfo, yMax);

                    werase(window);
                    printTodoList(window, &listoInfo);
                    break;

                case KEY_UP:
                    if (listoInfo.scrollOffset == 0 && listoInfo.selectedLine > 0)
                        listoInfo.selectedLine--;
                    else if (listoInfo.scrollOffset > 0)
                        listoInfo.scrollOffset--;
                        
                    getTodoListInfo(&listoInfo);

                    selectionCheck(&listoInfo, yMax);

                    werase(window);
                    printTodoList(window, &listoInfo);
                    break;

                // case '1':
                //     if (currentScreen != TODO_SCREEN){
                //         currentScreen = TODO_SCREEN;
                //         switchScreen(window, currentScreen);
                //     }
                //     break;

                // case '2':
                //     if (currentScreen != QUICK_ACCESS_SCREEN){
                //         currentScreen = QUICK_ACCESS_SCREEN;
                //         switchScreen(window, currentScreen);
                //     }
                //     break;

                default:
                    break;
            }
        }

    cleanup(&listoInfo);

    return 0;
}