#ifndef MYFLIE_H
#define MYFLIE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

uint8_t *myFileRead(char *filename, uint64_t *len);
int myFileWrite(char *filename, uint8_t *buf, uint64_t len);

#endif
