#define INTERNAL
#include "./definitions.h"

void u32string_uppercase(char32_t string[static 1]) {
    for (; *string != U'\0'; ++string) {
        switch (*string) {
            case U'a':
                *string = U'A';
                break;
            case U'b':
                *string = U'B';
                break;
            case U'c':
                *string = U'C';
                break;
            case U'd':
                *string = U'D';
                break;
            case U'e':
                *string = U'E';
                break;
            case U'f':
                *string = U'F';
                break;
            case U'g':
                *string = U'G';
                break;
            case U'h':
                *string = U'H';
                break;
            case U'i':
                *string = U'I';
                break;
            case U'j':
                *string = U'J';
                break;
            case U'k':
                *string = U'K';
                break;
            case U'l':
                *string = U'L';
                break;
            case U'm':
                *string = U'M';
                break;
            case U'n':
                *string = U'N';
                break;
            case U'o':
                *string = U'O';
                break;
            case U'p':
                *string = U'P';
                break;
            case U'q':
                *string = U'Q';
                break;
            case U'r':
                *string = U'R';
                break;
            case U's':
                *string = U'S';
                break;
            case U't':
                *string = U'T';
                break;
            case U'u':
                *string = U'U';
                break;
            case U'v':
                *string = U'V';
                break;
            case U'w':
                *string = U'W';
                break;
            case U'x':
                *string = U'X';
                break;
            case U'y':
                *string = U'Y';
                break;
            case U'z':
                *string = U'Z';
                break;

            default:
                break;
        }
    }
}