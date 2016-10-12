#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <time.h>

using namespace std;


enum Cell {
    ON,
    OFF
};

void cursesinit(WINDOW* mainwin){
    if ( (mainwin = initscr()) == NULL ) {
	fprintf(stderr, "Error initialising ncurses.\n");
	exit(EXIT_FAILURE);
    }
    start_color();
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
}

void cleanup(WINDOW* mainwin){
    delwin(mainwin);
    endwin();
}

void fill_array(Cell* arr, int size){
    srand(time(NULL));
    for (int i = 0; i < size; ++i){
        arr[i] = Cell(rand()%2);
        // arr[i] = OFF;
    }
    // arr[size/2] = ON;
}

void print_array(Cell* arr, int size){
    for (int i = 0; i < size; ++i){
        if (arr[i] == ON){
            attron(COLOR_PAIR(1));
        } else {
            attron(COLOR_PAIR(2));
        }
        addch(' '|A_REVERSE);
    }
}

void gen_array(Cell* arr, int size, Cell* newarr, Cell* rules){
    for (int i = 0; i < size; ++i){
        Cell a,b,c;
        b = arr[i];
        a = arr[(i-1)%size];
        c = arr[(i+1)%size];

        if (a == ON && b == ON && c == ON){
            newarr[i] = rules[0];
        } else if (a == ON && b == ON && c == OFF){
            newarr[i] = rules[1];
        } else if (a == ON && b == OFF && c == ON){
            newarr[i] = rules[2];
        } else if (a == ON && b == OFF && c == OFF){
            newarr[i] = rules[3];
        } else if (a == OFF && b == ON && c == ON){
            newarr[i] = rules[4];
        } else if (a == OFF && b == ON && c == OFF){
            newarr[i] = rules[5];
        } else if (a == OFF && b == OFF && c == ON){
            newarr[i] = rules[6];
        } else if (a == OFF && b == OFF && c == OFF){
            newarr[i] = rules[7];
        }
    }
}

void print_first(Cell* a, int WIDTH){
    fill_array(a, WIDTH);
    print_array(a, WIDTH);
}

void print_rest(Cell* a, Cell* b, int WIDTH, int HEIGHT, Cell* rules){
    int curr = 1;

    for (int i = 0; i < HEIGHT-1; ++i){
        if (curr == 1){
            gen_array(a, WIDTH, b, rules);
            print_array(b, WIDTH);
            curr = 0;
        } else {
            gen_array(b, WIDTH, a, rules);
            print_array(a, WIDTH);
            curr = 1;
        } 
    }
}


int main(int argc, char** argv){

    int HEIGHT = 0;
    int WIDTH = 0;

    WINDOW * mainwin;

    cursesinit(mainwin);
    
    getmaxyx(stdscr, HEIGHT, WIDTH);

    Cell* a = new Cell [WIDTH];
    Cell* b = new Cell [WIDTH];
    Cell rules[] = {ON, OFF, OFF, ON, OFF, ON, ON, OFF};

    print_first(a, WIDTH);
    
    print_rest(a, b, WIDTH, HEIGHT, rules);

    refresh();
    getch();
    cleanup(mainwin);
    return EXIT_SUCCESS;
}
