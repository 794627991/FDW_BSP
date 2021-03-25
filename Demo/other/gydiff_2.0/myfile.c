#include "myfile.h"

/* filename:要打开的文件名 len:打开的文件长度 返回读出数据的缓存地址 */
uint8_t *myFileRead(char *filename, uint64_t *len)
{
    uint8_t *tobuf;
    int fd, r;
    uint64_t i;
    if (((fd = open(filename, O_RDONLY | O_BINARY | O_NOINHERIT, 0)) < 0) ||
        ((*len = lseek(fd, 0, SEEK_END)) == -1) ||
        ((tobuf = (uint8_t *)malloc(*len + 1)) == NULL) ||
        (lseek(fd, 0, SEEK_SET) != 0))
    {
        printf("read %s fail\r\n", filename);
        return 0;
    }

    r = *len;
    while (r > 0 && (i = read(fd, tobuf + *len - r, r)) > 0)
        r -= i;
    if (r > 0 || close(fd) == -1)
    {
        printf("close %s fail\r\n", filename);
        return 0;
    }
    return tobuf;
}

int myFileWrite(char *filename, uint8_t *buf, uint64_t len)
{
    int fd;
    if (((fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, 0666)) < 0) ||
        (write(fd, buf, len) != len) || (close(fd) == -1))
    {
        printf("write %s fail\r\n", filename);
        return 0;
    }
    return 1;
}
