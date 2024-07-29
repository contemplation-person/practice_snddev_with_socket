#include "libari.h"

# define COLOR_RED_CODE 1
# define COLOR_GREEN_CODE 2
# define COLOR_YELLOW_CODE 3
# define COLOR_BLUE_CODE 4
# define COLOR_MAGENTA_CODE 5
# define COLOR_CYAN_CODE 6
# define COLOR_RESET_CODE 7

void ari_title_print(const char *string, const int color_code)
{
    char *color_string;

    switch (color_code)
    {
        case COLOR_RED_CODE:
            color_string = ASCII_COLOR_RED;
            break;
        case COLOR_GREEN_CODE:
            color_string = ASCII_COLOR_GREEN;
            break;
        case COLOR_YELLOW_CODE:
            color_string = ASCII_COLOR_YELLOW;
            break;
        case COLOR_BLUE_CODE:
            color_string = ASCII_COLOR_BLUE;
            break;
        case COLOR_MAGENTA_CODE:
            color_string = ASCII_COLOR_MAGENTA;
            break;
        case COLOR_CYAN_CODE:
            color_string = ASCII_COLOR_CYAN;
            break;
        case COLOR_RESET_CODE:
            color_string = ASCII_COLOR_RESET;
    }
    
    ari_putendl_fd(color_string, 1);
    ari_putstr_fd(string, 1);
    ari_putendl_fd(ASCII_COLOR_RESET, 1);
}
