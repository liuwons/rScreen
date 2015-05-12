#include <stdio.h>

#include "utils.h"
#include "encode.h"

using namespace rs;

int main(int argc, char** argv)
{
    int time_secs = 100;
    char fname[256];
    memset(fname, 0, 256);

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--time") == 0)
            time_secs = atoi(argv[i + 1]);
        else if (strcmp(argv[i], "--file") == 0)
            strcpy(fname, argv[i + 1]);
    }

    printf("time:%d secs, file:%s\n", time_secs, fname);

	encode_screen(time_secs, default_proc_data_callback, (void*)fname);
    return 0;
}