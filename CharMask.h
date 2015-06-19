#ifndef CHARMASK_H
#define CHARMASK_H

/**
 * A class that holds the bitmask for the dotted LCD font.
 * 
 * @author Holger Grosenick
 */
class CharMask
{
public:
    /**
     * Max number of lines in the dot matrix.
     */
    static const unsigned NUM_LINES = 9;

    /**
      * Max number of columns in the dot matrix.
      */
    static const unsigned NUM_COLS = 6;

    /**
      * Init the bitmask directly from 8 bytes.
      * @param inputMask bitmask with one byte per line in the bitmask
      */
    CharMask(const unsigned char inputMask[8]);

    /**
      * Init the bitmask from a 2-dimensional String array
      * @param mask a string array which defines the bitmask for each dotted char.
      */
    CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7, const char *m8);
    CharMask(const char *m1, const char *m2, const char *m3, const char *m4, const char *m5, const char *m6, const char *m7);

    /**
     * Default init: all bits are false -> this is a blank.
     */
    CharMask();

    CharMask(const CharMask& other) {
        *this = other;
    }

    CharMask& operator=(const CharMask &other);

    /**
     * Returns true is a dot of the char display is on.
     * @param x dot x 0..5
     * @param y dot y 0..8
     * @return true or false
     */
    bool isOn(unsigned x, unsigned y) const {
        return bitMask[x][y];
    }

private:
    /**
     * The dot matrix
     */
    bool bitMask[NUM_COLS][NUM_LINES];

    /**
     * Set the bitMask by char.
     */
    void init(const char **mask);
};

// this class is more or less const
typedef const CharMask *PCharMask;

#endif
