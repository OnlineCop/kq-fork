#pragma once

struct RGB
{
    unsigned char r, g, b, a;
};
#define PAL_SIZE 256
typedef RGB PALETTE[PAL_SIZE];
extern PALETTE black_palette;
void get_palette(RGB*);
void set_palette(RGB*);
void set_palette_range(RGB*, int, int);
struct COLOR_MAP
{
    char data[PAL_SIZE][PAL_SIZE];
};
inline int makecol(int r, int g, int b)
{
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}
extern int _color_depth;

typedef void FONT;
extern FONT* font;
inline int text_height(FONT*)
{
    return 0;
}
inline int text_length(FONT*, const char*)
{
    return 0;
}

inline int readkey()
{
    return 0;
}
inline int keypressed()
{
    return 0;
}
extern const unsigned char* key;
extern int key_count;
void fade_interpolate(RGB*, RGB*, RGB*, int, int, int);

#define DRAW_MODE_SOLID 0
#define DRAW_MODE_TRANS 1
inline void drawing_mode(int, void*, int, int)
{
}

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#define MAYBE true | false /* I'm joking of course */
