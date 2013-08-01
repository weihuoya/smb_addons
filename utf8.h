#pragma once
#ifndef _UTF8_H_
#define _UTF8_H_

/* Return code if invalid input after a shift sequence of n bytes was read.
   (xxx_mbtowc) */
#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))
/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)       (-2-2*(n))

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2

typedef unsigned int ucs4_t;

#ifdef __cplusplus
extern "C" {
#endif


int utf8_mbtowc (ucs4_t *pwc, const unsigned char *s, int n);
int utf8_wctomb (unsigned char *r, int n, ucs4_t wc);


#ifdef __cplusplus
}
#endif

#endif