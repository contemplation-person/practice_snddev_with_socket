#include "libari.h"

size_t ari_putstr_fd(const char *s, int fd)
{
    char *error = "error";

	if (!s || fd < 0)
    {
        write(2, error , strlen("error"));

		return -1;
    }

    return write(fd, s, strlen(s));
}
