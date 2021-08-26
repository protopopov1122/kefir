typedef unsigned int char32_t;

struct char32_string {
    char32_t content[32];
};

void char32_greet(struct char32_string *wstr) {
    *wstr = (struct char32_string){U'H', U'e', U'l', U'l', U'o', U',', U' ', U'w', U'o', U'r', U'l', U'd', U'!', U'\0'};
}