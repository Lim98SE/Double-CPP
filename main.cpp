#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <string>
#include <math.h>

std::vector<unsigned char> program = {};
std::vector<int> stack = {};

unsigned char memory[0xFF][0xFF];

unsigned int pointer = 0;
bool running = true;

unsigned char x_ptr = 0;
unsigned char y_ptr = 0;

unsigned char acc = 0;

unsigned char get_memory_byte() {
    return memory[x_ptr][y_ptr];
}

void set_memory_byte(unsigned char value) {
    memory[x_ptr][y_ptr] = (unsigned char)value;
}

unsigned char get_from_program() {
    unsigned int last = pointer;
    pointer++;

    if (pointer > program.size()) {
        running = false;
        return 0;
    }

    return program[last];
}

void init_memory() {
    for (int x = 0; x < 0xFF; x++) {
        for (int y = 0; y < 0xFF; y++) {
            memory[x][y] = 0;
        }
    }
}

void stack_push(int value) {
    stack.push_back(value);
}

int stack_pop() {
    int back = stack.back();
    stack.pop_back();
    return back;
}

int main() {
    // load program

    std::ifstream inFile("program.bin", std::ios::binary);

    // thanks https://stackoverflow.com/questions/36658734/c-get-all-bytes-of-a-file-in-to-a-char-array/36659103#36659103

    inFile.seekg(0, std::ios_base::end);
    size_t length = inFile.tellg();
    inFile.seekg(0, std::ios_base::beg);

    program.reserve(length);

    std::copy( std::istreambuf_iterator<char>(inFile),
           std::istreambuf_iterator<char>(),
           std::back_inserter(program) );

    init_memory();

    srand(time(NULL));

    while (running) {
        if (pointer >= sizeof(program)) { // cut off program if we reach the end
            std::cout << "Exited: ran out of program\n";
            running = false;
            break;
        }

        unsigned char opcode = get_from_program();

        if (!running) {
            break;
        }

        switch (opcode) {
            case 0: { // print value (PV)
                std::cout << (int)memory[x_ptr][y_ptr] << std::endl;
                break;
            }

            case 1: { // print character (PC)
                std::cout << memory[x_ptr][y_ptr];
                break;
            }
            
            case 2: { // set x (SX)
                x_ptr = get_from_program();
                break;
            }
            
            case 3: { // set y (SY)
                y_ptr = get_from_program();
                break;
            }
            
            case 4: { // increment x (IX)
                x_ptr ++;
                break;
            }
            
            case 5: {
                y_ptr ++;
                break;
            } // increment y (IY)

            case 6: {
                x_ptr --;
                break;
            } // decrement x (DX)
            
            case 7: {
                y_ptr --;
                break;
            } // decrement y (DY)
            
            case 8: {
                set_memory_byte(get_from_program());
                break;
            } // set value (SV)

            case 9: {
                memory[x_ptr][y_ptr]++;
                break;
            } // increment value (IV)
            
            case 10: {
                memory[x_ptr][y_ptr]--;
                break;
            } // decrement value (DV)
            
            case 11: {
                pointer = 0;
                break;
            } // restart (RS)
            
            case 12: {
                unsigned char condition = get_from_program();
                if (get_memory_byte() != condition) {
                    pointer = 0;
                }

                break;
            } // conditional restart (CR)
            
            case 13: {
                unsigned char inp;
                std::cout << "Char? ";
                std::cin >> inp;

                set_memory_byte(inp);
            } // get char (GC)
            
            case 14: {
                int base_inp;
                std::cout << "Num? ";
                std::cin >> base_inp;

                if (base_inp < 0 || base_inp > 0xFF) {
                    std::cout << "Number's too big.";
                    return 0;
                }

                set_memory_byte((unsigned char) base_inp);

                break;
            } // get value (GV)

            case 15: {
                set_memory_byte(x_ptr);
                break;
            } // set value to x (XV)

            case 16: {
                set_memory_byte(y_ptr);
                break;
            } // set value to y (YV)

            case 17: {
                unsigned int to = get_from_program();
                pointer = to;
                break;
            } // jump (JM)

            case 18: {
                unsigned char condition = get_from_program();
                unsigned char to = get_from_program();

                if (get_memory_byte() != condition) {
                    pointer = to;
                }

                break;

            } // conditional jump (CJ)

            // ** was removed, nothing of note was lost

            // 1.1 features

            case 20: {
                unsigned char by = get_from_program();

                pointer += by;
                break;
            } // jump forward (JF)

            case 21: {
                unsigned char by = get_from_program();

                pointer -= by;
                pointer %= program.size();
                break;
            } // jump backward (JB)

            case 22: {
                unsigned char condition = get_from_program();
                unsigned char by = get_from_program();

                if (condition != get_memory_byte()) {
                    pointer += by;
                    pointer %= program.size();
                }

                break;
            } // cond. jump forward (CF)

            case 23: {
                unsigned char condition = get_from_program();
                unsigned char by = get_from_program();

                if (condition != get_memory_byte()) {
                    pointer -= by;
                    pointer %= program.size();
                }

                break;
            } // cond. jump backward (CB)

            // 1.2 features

            case 24: {
                std::string inp;
                std::cout << "Str? ";
                std::getline(std::cin, inp);

                for (int i = 0; i < inp.length(); i++) {
                    unsigned char byte = inp[i];
                    set_memory_byte(byte);
                    x_ptr ++;
                }

                set_memory_byte(0xFF);
                break;

            } // get string (GS)

            // 1.3 features

            case 25: {
                unsigned char address = get_from_program();

                stack_push(pointer);
                pointer = (int)address;
                break;
            } // jump to subroutine (JR)

            case 26: {
                int to_return_to = stack_pop();
                pointer = to_return_to;
                break;
            } // return from subroutine (RR)

            case 27: {
                unsigned char condition = get_from_program();
                unsigned char address = get_from_program();

                if (condition != get_memory_byte()) {
                    stack_push(pointer);
                    pointer = (int)address;
                }

                break;
            } // jump to subr. cond. (RC)

            case 28: {
                unsigned char condition = get_from_program();

                if (condition != get_memory_byte()) {
                    pointer = stack_pop();
                }

                break;
            } // return from subr. cond. (BC)

            case 29: {
                set_memory_byte((unsigned char)(rand() % 0xFF));
                break;
            } // random number (RN)

            case 31: {
                unsigned char current = get_from_program();

                while (current != 0xFF) {
                    set_memory_byte(current);
                    x_ptr ++;
                    current = get_from_program();
                }

                set_memory_byte(current);

                break;
            } // data block

            case 32: {
                unsigned char current = get_memory_byte();

                do {
                    current = get_memory_byte();

                    if (current == 0xFF) {
                        break;
                    }

                    std::cout << current;
                    x_ptr ++;
                } while (current != 0xFF);

                break;
            } // print string (PS)

            case 33: {
                acc += get_memory_byte();
                break;
            } // add to accumulator (+C)

            case 34: {
                acc -= get_memory_byte();
                break;
            } // sub from accumulator (-C)

            case 35: {
                acc ++;
                break;
            } // increment accumulator (IC)

            case 36: {
                acc --;
                break;
            } // decrement accumulator (-C)

            case 37: {
                acc = get_from_program();
                break;
            } // set accumulator (SA)

            case 38: {
                set_memory_byte(acc);
                break;
            } // get accumulator value (AV)

            case 39: {
                stack_push(get_memory_byte());
                break;
            } // push to stack (PH)

            case 40: {
                set_memory_byte(stack_pop());
                break;
            } // pop from stack (PL)

            case 41: {
                return 0;
            } // exit (XT)
        }
    }
}