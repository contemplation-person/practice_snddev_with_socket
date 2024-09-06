#include "libari.h"

const char *put_color_string(const int color_code)
{
    switch (color_code)
    {
        case COLOR_RED_CODE:
            return ASCII_COLOR_RED;
        case COLOR_GREEN_CODE:
            return ASCII_COLOR_GREEN;
        case COLOR_YELLOW_CODE:
            return ASCII_COLOR_YELLOW;
        case COLOR_BLUE_CODE:
            return ASCII_COLOR_BLUE;
        case COLOR_MAGENTA_CODE:
            return ASCII_COLOR_MAGENTA;
        case COLOR_CYAN_CODE:
            return ASCII_COLOR_CYAN;
        default:
            return ASCII_COLOR_RESET;
    }
}

void ari_title_print(const char *string, const int color_code)
{
    const char *color_string = put_color_string(color_code);
    
    ari_putendl_fd(color_string, 1);
    ari_putstr_fd(string, 1);
    ari_putendl_fd(ASCII_COLOR_RESET, 1);
}

void ari_title_print_fd(const int fd, const char *string, const int color_code)
{
    const char *color_string = put_color_string(color_code);
    
    ari_putendl_fd(color_string, fd);
    ari_putstr_fd(string, fd);
    ari_putendl_fd(ASCII_COLOR_RESET, fd);
}

void    ari_print_error(const char *string, const char *file, const int line)
{
    fprintf(stderr, "%s%s - %s:%d%s\n", \
            ASCII_COLOR_RED, string, file, line, ASCII_COLOR_RESET);
    
}
