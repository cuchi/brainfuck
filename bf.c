#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

long to_next_bracket(long pos, char *array, long size) {
    int n = 0;
    while (pos < size) {
        if (array[pos] == '[') {
            n++;
        } else if (array[pos] == ']') {
            if (n == 0) {
                return pos;
            } else {
                n--;
            }
        }
        pos++;
    }
    return -1;
}



int main(int argc, char **argv) {
    FILE *input_file; 
    char *input;
    long pos = 0;
    long input_size;
    char _bf_vec[1024*1024] = {0};
    char *bf_vec = _bf_vec;
    long *stack = malloc(1024 * sizeof(long));
    int stack_top = -1;
    char command;
    
    if (argc != 2) {
        printf("### Usage:\n\n");
        printf("%s input_file.bf\n\n", argv[0]);
        return 1;
    }

    input_file = fopen(argv[1], "r");

    if (input_file == NULL) {
        fprintf(stderr, "Error opening file.");
        return 2;
    }

    fseek(input_file, 0, SEEK_END);
    input_size = ftell(input_file);
    rewind(input_file);
    input = malloc(input_size + 1);
    fread(input, input_size, 1, input_file);
    fclose(input_file);
    

    // terminal handling to get getchar working properly
    // thanks to http://stackoverflow.com/questions/1798511/
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    pos = 0;
    while(pos < input_size) {
        command = input[pos];
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
            if (!*bf_vec) {
                pos = to_next_bracket(pos + 1, input, input_size);
            } else {
                stack[++stack_top] = pos;
            }
        } else if (command == ']') {
            if (*bf_vec) {
                //fseek(f, stack[stack_top], SEEK_SET);
                pos = stack[stack_top];
            } else {
                stack_top--;
            }
        }
        pos++;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return 0;
}
