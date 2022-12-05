#include <stdlib.h>
#include <stdio.h>
#include <pwd.h> // for getpwuid()
#include <unistd.h> // for getuid()
#include <sys/stat.h> // for mkdir()
#include <sys/types.h> // for mkdir()

#include "utils.h"

void cleanup(ListoInfo *listoInfo){
    endwin();

    if (listoInfo->saveLocation != NULL)
        free(listoInfo->saveLocation);
    if (listoInfo->buffer != NULL)
        free(listoInfo->buffer);
    if (listoInfo->tempBuffer != NULL)
        free(listoInfo->tempBuffer);
    if (listoInfo->taskInfo != NULL && listoInfo->taskInfo->title != NULL)
        free(listoInfo->taskInfo->title);
    if (listoInfo->taskInfo != NULL)
        free(listoInfo->taskInfo);
}

char *getWindowName(int screen){
    char *name;
    switch(screen){
        case TODO_SCREEN:
            name = "Todo List";
            break;

        case QUICK_ACCESS_SCREEN:
            name = "Quick Access";
            break;

        default:
            name = "";
            break;
    }

    return name;
}

void createBox(int screen){
    box(stdscr, 0, 0);
    mvwprintw(stdscr, 0, 2, " %s ", getWindowName(screen));
}

void switchScreen(WINDOW *window, int screen){
    werase(window);
    createBox(screen);
    wrefresh(window);
}

void initColors(ListoInfo *listoInfo){
    if (!has_colors()){
        cleanup(listoInfo);
        printf("Terminal does not support colors!\n");
        exit(0);
    }

    start_color();
    use_default_colors();

    if (can_change_color()){
        init_color(COLOR_CYAN, HEX(0), HEX(155), HEX(255));
        init_color(COLOR_YELLOW, HEX(255), HEX(130), HEX(25));
        init_color(COLOR_GREEN, HEX(0), HEX(210), HEX(0));
    }

    init_pair(COLOR_IMPORTANT, COLOR_CYAN, -1);
    init_pair(COLOR_IMPORTANT_REVERSE, COLOR_BLACK, COLOR_CYAN);

    init_pair(COLOR_QUESTIONABLE, COLOR_YELLOW, -1);
    init_pair(COLOR_QUESTIONABLE_REVERSE, COLOR_BLACK, COLOR_YELLOW);

    init_pair(COLOR_DONE, COLOR_GREEN, -1);
    init_pair(COLOR_DONE_REVERSE, COLOR_BLACK, COLOR_GREEN);
}

void selectionCheck(ListoInfo *listoInfo, int yMax){
    if (listoInfo->selectedLine < 0)
        listoInfo->selectedLine = 0;
    if (listoInfo->scrollOffset < 0)
        listoInfo->scrollOffset = 0;
    if (listoInfo->selectedLine > yMax/2)
        listoInfo->selectedLine = yMax/2;
    if ((listoInfo->selectedLine + listoInfo->scrollOffset) >= listoInfo->todoListSize && listoInfo->todoListSize <= yMax/2)
        listoInfo->selectedLine = listoInfo->todoListSize-1;
    if ((listoInfo->selectedLine + listoInfo->scrollOffset) >= listoInfo->todoListSize && listoInfo->todoListSize > yMax/2){
        listoInfo->selectedLine = yMax/2;
        listoInfo->scrollOffset = listoInfo->todoListSize-1 - yMax/2;
    }
}

void findSaveLocation(char **buffer){
    FILE *fp;
    char *userHome;
    int size = 0;

    if ((userHome = getenv("HOME")) == NULL)
        userHome = getpwuid(getuid())->pw_dir;

    size = (strlen(userHome)+20)*sizeof(char);
    *buffer = (char *)malloc(size);

    snprintf(*buffer, size, "%s/.listo", userHome);

    mkdir(*buffer, 0755);

    snprintf(*buffer, size, "%s/.listo/todolist", userHome);

    ensureTodoList(buffer);
}

void ensureTodoList(char **location){
    FILE *fp;

    if ((fp = fopen(*location, "r")) != NULL){
        fclose(fp);
    }
    else{
        fp = fopen(*location, "w");

        fprintf(fp, "This is an example todo list\n-!Important\n-?Questionable\n-~Done\n!Controls\n-!Up: Scroll up\n-!Down: Scroll down\n-~N/n: Create new task\n-~Delete: Delete selected task\n~Creating a new task\n-!Set the title\n-?Set the property (None/Important/Questionable/Done)\n-?Set wether it is a bullet or not\n-?Choose a placement Above/Below the selected task");

        fclose(fp);
    }
}

void getTodoListInfo(ListoInfo *listoInfo){
    FILE *fp;
    int curOffset = 0;
    listoInfo->todoListSize = 0;
    listoInfo->todoListNotPrinted = 0;

    ensureTodoList(&listoInfo->saveLocation);

    fp = fopen(listoInfo->saveLocation, "r");
    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fp)){
        curOffset++;
        if (listoInfo->buffer[0] == '\n'){
            if (curOffset < listoInfo->scrollOffset+1)
                listoInfo->todoListNotPrinted++;
            continue;
        }
        listoInfo->todoListSize++;
    }
    fclose(fp);
}

int readTodoList(ListoInfo *listoInfo, int offset, int ignoreEmpty, int *nonBullets){
    FILE *fp;
    int curOffset = 0;

    *nonBullets = 0;
    
    ensureTodoList(&listoInfo->saveLocation);

    fp = fopen(listoInfo->saveLocation, "r");
    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fp)){
        curOffset++;
        if (listoInfo->buffer[0] == '\n' && ignoreEmpty) curOffset--;
        if (curOffset > offset) break;
        if (listoInfo->buffer[0] != '-' && listoInfo->buffer[0] != '\n') (*nonBullets)++;
    }
    fclose(fp);

    if (curOffset == offset){
        snprintf(listoInfo->buffer, TEXT_BUFFER_SIZE, "");
        return 1;
    }

    return 0;
}

void writeTodoList(ListoInfo *listoInfo, char **input, int index){
    FILE *fp, *fpTemp;
    int lineCount = 0;
    
    ensureTodoList(&listoInfo->saveLocation);

    fp = fopen(listoInfo->saveLocation, "r");
    fpTemp = fopen("/tmp/listo.tmp", "w");

    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fp)){
        fprintf(fpTemp, "%s", listoInfo->buffer);
    }
    fprintf(fpTemp, "\n");

    fclose(fp);
    fclose(fpTemp);

    fp = fopen(listoInfo->saveLocation, "w");
    fpTemp = fopen("/tmp/listo.tmp", "r");

    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fpTemp)){
        if (lineCount == index)
            fprintf(fp, "%s\n", *input);

        if (listoInfo->buffer[0] != '\n'){
            fprintf(fp, "%s", listoInfo->buffer);
            lineCount++;
        }
    }

    fclose(fp);
    fclose(fpTemp);
}

void deleteTodoList(ListoInfo *listoInfo, int index){
    FILE *fp, *fpTemp;
    int lineCount = 0;
    
    ensureTodoList(&listoInfo->saveLocation);

    fp = fopen(listoInfo->saveLocation, "r");
    fpTemp = fopen("/tmp/listo.tmp", "w");

    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fp)){
        fprintf(fpTemp, "%s", listoInfo->buffer);
    }
    fprintf(fpTemp, "\n");

    fclose(fp);
    fclose(fpTemp);

    fp = fopen(listoInfo->saveLocation, "w");
    fpTemp = fopen("/tmp/listo.tmp", "r");

    while (fgets(listoInfo->buffer, TEXT_BUFFER_SIZE, fpTemp)){
        if (listoInfo->buffer[0] != '\n' && lineCount != index){
            fprintf(fp, "%s", listoInfo->buffer);
        }

        if (listoInfo->buffer[0] != '\n')
            lineCount++;
    }

    fclose(fp);
    fclose(fpTemp);
}

void printTodoList(WINDOW *window, ListoInfo *listoInfo){
    int items = 0;
    int nonBullets;
    int notPrinted = listoInfo->todoListNotPrinted;
    int numberLength;
    int color;
    int bullet;
    attr_t attribute;
    
    while(true){
        if (readTodoList(listoInfo, items+listoInfo->scrollOffset+notPrinted, 0, &nonBullets) == 1 || items > window->_maxy) break;
        if (strcmp(listoInfo->buffer, "") == 0 || listoInfo->buffer[0] == '\n') {
            notPrinted++;
            continue;
        }

        bullet = 0;
        if (listoInfo->buffer[0] == '-') bullet = 1;

        color = -1;
        if ((int)listoInfo->buffer[0+bullet] == COLOR_IMPORTANT){
            color = COLOR_IMPORTANT;
            attribute = ATTR_IMPORTANT;
        }
        if ((int)listoInfo->buffer[0+bullet] == COLOR_QUESTIONABLE){
            color = COLOR_QUESTIONABLE;
            attribute = ATTR_QUESTIONABLE;
        }
        if ((int)listoInfo->buffer[0+bullet] == COLOR_DONE){
            color = COLOR_DONE;
            attribute = ATTR_DONE;
        }

        strcpy(listoInfo->tempBuffer, listoInfo->buffer+bullet+(color!=-1));
        strcpy(listoInfo->buffer, listoInfo->tempBuffer);

        if (strlen(listoInfo->buffer) > window->_maxx - ((!bullet) ? NON_BULLET_X : BULLET_X) - 3){
            listoInfo->buffer[window->_maxx - ((!bullet) ? NON_BULLET_X : BULLET_X) - 3] = '\0';
            listoInfo->buffer[window->_maxx - ((!bullet) ? NON_BULLET_X : BULLET_X) - 4] = '.';
            listoInfo->buffer[window->_maxx - ((!bullet) ? NON_BULLET_X : BULLET_X) - 5] = '.';
            listoInfo->buffer[window->_maxx - ((!bullet) ? NON_BULLET_X : BULLET_X) - 6] = '.';
        }

        if (!bullet){
            wattron(window, A_BOLD);
            mvwprintw(window, items, 0, "%*i", NON_BULLET_X, ++nonBullets);
            wattroff(window, A_BOLD);
            wprintw(window, ". ");
        }
        else
            mvwprintw(window, items, BULLET_X, "• ");

        if (items == listoInfo->selectedLine)
            wattron(window, (color != -1) ? COLOR_PAIR(color+1) | attribute : A_REVERSE);
        else if (color != -1) wattron(window, COLOR_PAIR(color) | attribute);
        wprintw(window, "%s", listoInfo->buffer);
        if (items == listoInfo->selectedLine)
            wattroff(window, (color != -1) ? COLOR_PAIR(color+1) | attribute : A_REVERSE);
        else if (color != -1) wattroff(window, COLOR_PAIR(color) | attribute);

        items++;
    }

    wrefresh(window);
}