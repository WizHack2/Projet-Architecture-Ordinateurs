#include "minirisc.h"
#include "platform.h"

minirisc_t* minirisc_new(uint32_t initial_PC, platform_t *platform) {

    minirisc_t* minirisc;
    minirisc = (minirisc_t*) malloc(sizeof(minirisc_t));

    minirisc->PC = initial_PC;
	minirisc->IR = 0;
    minirisc->next_PC = 0;
    for (int i = 0; i < 32; i++) {
        minirisc->regs[i] = 0;
    }
    minirisc->platform = platform;
    minirisc->halt = 0; 

    return minirisc;
}

// TODO On passe maintenant a minirisc.c en commencant par la fonction minirisc_fetch