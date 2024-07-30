#include "libari.h"

size_t	ari_putendl_fd(const char *s, int fd)
{
    size_t str_cnt = write(fd, s, strlen(s));
    return str_cnt + write(fd, "\n", 1);
}
