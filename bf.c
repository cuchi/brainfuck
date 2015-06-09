#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char **argv) {
    FILE *f = fopen(argv[1], "r"); 

    if (f == NULL) {
        return 1;
    }

    char _bf_vec[1024] = {0};
    char *bf_vec = _bf_vec;
    long *stack = malloc(1024 * sizeof(long));
    int stack_top = -1;
    char command;

    // terminal handling to get getchar working properly
    // thanks to http://stackoverflow.com/questions/1798511/
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while(fread(&command, 1, 1, f)) {
        if(command == '>') {
            bf_vec++;
        } else if (command == '<') {
            bf_vec--;
        } else if (command == '+') {
            (*bf_vec)++;
        } else if (command == '-') {
            (*bf_vec)--;
        } else if (command == ',') {
            *bf_vec = getchar();
        } else if (command == '.') {
            putchar(*bf_vec);
        } else if (command == '[') {
            stack[++stack_top] = ftell(f);
        } else if (command == ']') {
            if (*bf_vec) {
                fseek(f, stack[stack_top], SEEK_SET);
            } else {
                stack_top--;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return 0;
}
