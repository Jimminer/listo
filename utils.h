#ifndef _UTILS_H
#define _UTILS_H

#include <ncurses.h>
#include <string.h>

#define MINIMUM_Y 30
#define MINIMUM_X 90

#define INPUT_INTERVAL 30

#define COLOR_IMPORTANT ((int)'!')
#define COLOR_QUESTIONABLE ((int)'?')
#define COLOR_DONE ((int)'~')

#define COLOR_IMPORTANT_REVERSE ((int)'!'+1)
#define COLOR_QUESTIONABLE_REVERSE ((int)'?'+1)
#define COLOR_DONE_REVERSE ((int)'~'+1)

#define ATTR_IMPORTANT (A_BOLD | A_UNDERLINE)
#define ATTR_QUESTIONABLE A_ITALIC
#define ATTR_DONE 0

#define NON_BULLET_X 4
#define BULLET_X 8

#define HEX(n) (n/255.0*1000)
#define TEXT_BUFFER_SIZE (1024*sizeof(char))

typedef struct ListoInfo ListoInfo;
typedef struct TaskInfo TaskInfo;

struct ListoInfo{
    char *saveLocation;
    char *buffer;
    char *tempBuffer;
    int scrollOffset;
    int selectedLine;
    int todoListSize;
    int todoListNotPrinted;
    TaskInfo *taskInfo;
};

struct TaskInfo{
    char *title;
    int index;
    int placement;
    int property;
    int bullet;
    int valid;
};

enum SCREEN{
    TODO_SCREEN,
    QUICK_ACCESS_SCREEN
};

void cleanup(ListoInfo *listoInfo);
char *getWindowName(int screen);
void createBox(int screen);
void switchScreen(WINDOW *window, int screen);
void initColors(ListoInfo *listoInfo);
void selectionCheck(ListoInfo *listoInfo, int yMax);
void findSaveLocation(char **buffer);
void ensureTodoList(char **location);
void getTodoListInfo(ListoInfo *listoInfo);
int readTodoList(ListoInfo *listoInfo, int offset, int ignoreEmpty, int *nonBullets);
void writeTodoList(ListoInfo *listoInfo, char **input, int index);
void deleteTodoList(ListoInfo *listoInfo, int index);
void printTodoList(WINDOW *window, ListoInfo *listoInfo);

#endif