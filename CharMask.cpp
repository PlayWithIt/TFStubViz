
#include <string.h>
#include <strings.h>

#include "CharMask.h"

/**
 * Init the bitmask from a 2-dimensional String array
 */
CharMask::CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7, const char *m8)
{
    const char *mask[] = {m1, m2, m3, m4, m5, m6, m7, m8, NULL };
    init(mask);
}

/**
 * Init the bitmask from a 2-dimensional String array
 */
CharMask::CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7)
{
    const char *mask[] = {m1, m2, m3, m4, m5, m6, m7, NULL };
    init(mask);
}

/**
  * Init the bitmask directly from 8 bytes.
  * @param inputMask bitmask with one byte per line in the bitmask
  */
CharMask::CharMask(const unsigned char inputMask[])
{
    bzero(bitMask, sizeof(bitMask));
    const unsigned leftBit = 0b00010000;

    for (unsigned y = 0; y < 8; ++y)
    {
        unsigned char byte = inputMask[y];
        for (unsigned x = 0; x < NUM_COLS; ++x)
        {
            if ((byte & (leftBit >> x)) != 0)
                bitMask[x][y] = true;
        }
    }
}

CharMask& CharMask::operator=(const CharMask &other)
{
    memcpy(bitMask, other.bitMask, sizeof(bitMask));
    return *this;
}

void CharMask::init(const char** mask)
{
    bzero(bitMask, sizeof(bitMask));
    unsigned w = strlen(mask[0]);

    for (unsigned y = 0; mask[y] && y < NUM_LINES; ++y)
    {
        const char *s = mask[y];
        for (unsigned x = 0; x < w; ++x)
        {
            char ch = s[x];
            if (ch == 'X' || ch == 'x')
                bitMask[x][y] = true;
        }
    }
}

/**
 * Default init: all bits are false -> this is a blank.
 */
CharMask::CharMask()
{
    bzero(bitMask, sizeof(bitMask));
}
