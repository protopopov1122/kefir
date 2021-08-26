typedef unsigned short char16_t;

struct char16_string {
    char16_t content[32];
};

void char16_greet(struct char16_string *wstr) {
    *wstr = (struct char16_string){u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!', u'\0'};
}