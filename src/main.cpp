#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <cstring>
#include <boost/program_options.hpp>

namespace po = boost::program_options;


#define RULESIZE 8

using namespace std;


enum Cell {
    ON,
    OFF
};

enum Mode {
    RANDOM,
    TRIANGLE
};

int decimal_to_binary(int n){
    int rem, i=1, binary=0;
    while (n!=0)
    {
        rem=n%2;
        n/=2;
        binary+=rem*i;
        i*=10;
    }
    return binary;
}

void cursesinit(WINDOW* mainwin){
    if ( (mainwin = initscr()) == NULL ) {
	fprintf(stderr, "Error initialising ncurses.\n");
	exit(EXIT_FAILURE);
    }
    start_color();
    noecho();
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
}

void cleanup(WINDOW* mainwin){
    delwin(mainwin);
    endwin();
}

void fill_array(Cell* arr, int size, Mode mode){
    if (mode == RANDOM){
        srand(time(NULL));
        for (int i = 0; i < size; ++i){
            arr[i] = Cell(rand()%2-1);
        }
    } else if (mode == TRIANGLE){
        for (int i = 0; i < size; ++i){
            arr[i] = OFF;
        }
        arr[size/2] = ON;
    } 
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
        b = arr[i%size];
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

void print_first(Cell* a, int WIDTH, Mode mode){
    fill_array(a, WIDTH, mode);
    print_array(a, WIDTH);
}

void print_rest(Cell* a, Cell* b, int WIDTH, int HEIGHT, Cell* rules){
    int curr = 1;

    for (int i = 0; i < HEIGHT-1; ++i){
        // getch(); 
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

void default_pattern(Cell* rules){
    cout << "Defaulting to pattern: [10010110]\r" << endl;
    rules[0] = ON;
    rules[1] = OFF;
    rules[2] = OFF;
    rules[3] = ON;
    rules[4] = OFF;
    rules[5] = ON;
    rules[6] = ON;
    rules[7] = OFF;
}


void parse_args(int argc, char** argv, Cell* rules, Mode* mode){
    if (argc < 3){
        cout << "No mode specified defaulting to TRIANGLE\r" << endl;
        *mode = TRIANGLE;
    } else {
        if (strcmp(argv[2], "-r") == 0){
            cout << "MODE: RANDOM\r" << endl;
            *mode = RANDOM;
        } else if (strcmp(argv[2], "-t") == 0){
            cout << "MODE: TRIANGLE\r" << endl;
            *mode = TRIANGLE;
        } else {
            cout << "Invalid mode defaulting to TRIANGLE\r" << endl;
            *mode = TRIANGLE;
        }
    }
    if (argc < 2){
        cout << "Pattern not provided\r" << endl;
        default_pattern(rules);
    } else {
        if (stoi(argv[1]) > 255){
            cout << "Invalid Pattern size - Must be less than 255\r" << endl;
            default_pattern(rules);
            return;
        }   
        char pattern[RULESIZE+1];
        snprintf(pattern, sizeof(pattern), "%08d", decimal_to_binary(stoi(argv[1])));
        cout << "Using specified pattern [" << pattern << "]\r" << endl;
        for (int i = 0; i < strlen(pattern); ++i){
            if (pattern[i] == '1'){
                rules[i] = ON;
            } else if (pattern[i] == '0'){
                rules[i] = OFF;
            } else {
                cout << "Invalid Pattern\r" << endl;
                default_pattern(rules);
            }
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
    Cell* rules = new Cell [RULESIZE];
    Mode mode;

    parse_args(argc, argv, rules, &mode);

    print_first(a, WIDTH, mode);
    
    print_rest(a, b, WIDTH, HEIGHT, rules);

    refresh();
    getch();
    cleanup(mainwin);
    delete[] a;
    delete[] b;
    delete[] rules;

    return EXIT_SUCCESS;
}
