#include "bsdiff.h"
#include "bspatch.h"

int main(void)
{
    char argv1[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\11.bin"};
    char argv2[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\22.bin"};
    char argv3[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\0.dif"};
    char argv4[200] = {"C:\\Users\\gaoyi\\Desktop\\gydiff_2.0\\test\\33.bin"};
    diff((char *)&argv1, (char *)&argv2, (char *)&argv3);
    patch((char *)&argv1, (char *)&argv4, (char *)&argv3);
}
