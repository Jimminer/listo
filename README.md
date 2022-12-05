# listo

A simple todo list TUI written using NCurses

# Basics

- Tasks have different properties based on their status:
    > None, Important, Questionable, Done


- You can add/remove a task within the app (see controls below)

- Controls:

    >`Up Arrow`: Scrolls up

    >`Down Arrow`: Scrolls down

    >`q`: Closes the app

    >`n`: Creates a new task

    >`Delete`: Deletes the selected task

# Building

`ncursesw` is required for building (provides support for Unicode characters)

```
git clone https://github.com/Jimminer/listo.git
cd listo
make
```

# Future updates

- Split utils into more modules

- Implement edit popup

- Adapt to terminal size change