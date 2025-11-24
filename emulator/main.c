#include "platform.h"
#include "minirisc.h"

int main()
{
    platform_t *platform;
    minirisc_t *minirisc;

    platform = platform_new();
    minirisc = minirisc_new(0x80000000, platform);

    platform_load_program(platform, "path/to/program.bin");
    minirisc_run();

    minirisc_free(minirisc);
    platform_free(platform);

    return 0;
}