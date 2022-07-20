/*
 * Copyright © 2005-2020 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <errno.h>
#include "kefir/core/basic-types.h"
#include "kefir/util/uchar.h"
#if defined(KEFIR_OPENBSD_HOST_PLATFORM) || defined(KEFIR_NETBSD_HOST_PLATFORM)

size_t c32rtomb(char *restrict s, char32_t c32, mbstate_t *restrict ps) {
    return wcrtomb(s, c32, ps);
}

size_t mbrtoc32(char32_t *restrict pc32, const char *restrict s, size_t n, mbstate_t *restrict ps) {
    static unsigned internal_state;
    if (!ps)
        ps = (void *) &internal_state;
    if (!s)
        return mbrtoc32(0, "", 1, ps);
    wchar_t wc;
    size_t ret = mbrtowc(&wc, s, n, ps);
    if (ret <= 4 && pc32)
        *pc32 = wc;
    return ret;
}

size_t c16rtomb(char *restrict s, char16_t c16, mbstate_t *restrict ps) {
    static unsigned internal_state;
    if (!ps)
        ps = (void *) &internal_state;
    unsigned *x = (unsigned *) ps;
    wchar_t wc;

    if (!s) {
        if (*x)
            goto ilseq;
        return 1;
    }

    if (!*x && c16 - 0xd800u < 0x400) {
        *x = c16 - 0xd7c0 << 10;
        return 0;
    }

    if (*x) {
        if (c16 - 0xdc00u >= 0x400)
            goto ilseq;
        else
            wc = *x + c16 - 0xdc00;
        *x = 0;
    } else {
        wc = c16;
    }
    return wcrtomb(s, wc, 0);

ilseq:
    *x = 0;
    errno = EILSEQ;
    return -1;
}

size_t mbrtoc16(char16_t *restrict pc16, const char *restrict s, size_t n, mbstate_t *restrict ps) {
    static unsigned internal_state;
    if (!ps)
        ps = (void *) &internal_state;
    unsigned *pending = (unsigned *) ps;

    if (!s)
        return mbrtoc16(0, "", 1, ps);

    /* mbrtowc states for partial UTF-8 characters have the high bit set;
     * we use nonzero states without high bit for pending surrogates. */
    if ((int) *pending > 0) {
        if (pc16)
            *pc16 = *pending;
        *pending = 0;
        return -3;
    }

    wchar_t wc;
    size_t ret = mbrtowc(&wc, s, n, ps);
    if (ret <= 4) {
        if (wc >= 0x10000) {
            *pending = (wc & 0x3ff) + 0xdc00;
            wc = 0xd7c0 + (wc >> 10);
        }
        if (pc16)
            *pc16 = wc;
    }
    return ret;
}
#endif
