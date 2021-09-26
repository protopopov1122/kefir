#define INTERNAL
#include "./definitions.h"

void char16_greet(struct char16_string *wstr) {
    *wstr = (struct char16_string){u'H', u'e', u'l', u'l', u'o', u',', u' ', u'w', u'o', u'r', u'l', u'd', u'!', u'\0'};
}