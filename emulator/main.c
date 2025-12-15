#include "platform.h"
#include "minirisc.h"

int main() {

    // --- TEST ---
    // platform_test();
    minirisc_test();

    platform_t* platform;
    minirisc_t* minirisc;

    platform = platform_new();
    minirisc = minirisc_new(0x80000000, platform);

    // platform_load_program(platform, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/test1/build/esw.bin");
    platform_load_program(platform, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/fibonacci/build/esw.bin");
    minirisc_run(minirisc);

    minirisc_free(minirisc);
    platform_free(platform);

    return 0;
}