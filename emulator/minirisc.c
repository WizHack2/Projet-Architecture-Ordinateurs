#include <stdlib.h>

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

void minirisc_free(minirisc_t* mr) {
    free(mr);
}

void minirisc_fetch(minirisc_t *mr) {
    uint32_t new_IR;
    if (platform_read(mr->platform,ACCESS_WORD,mr->PC,&new_IR) == 0) {
        mr->IR = new_IR;
        mr->next_PC = mr->PC + 4;
    }
    else {
        mr->halt = 1;
    }
}

void minirisc_decode_and_execute(minirisc_t* mr) {
    uint32_t opcode = mr->IR & 0x7F;

    switch (opcode) {
        case 1: // LUI
            int RD =  (mr->IR >> 7) & 0x1F;
            mr->regs[RD] = mr->IR & 0xFFFFF000;
            break;
        case 2: //AUIPC
            //TODO AUIPC
        default:
            mr->halt = 1;
            break;
    }
}

void minirisc_run(minirisc_t* mr) {
    while (mr->halt == 0) {
        minirisc_fetch(mr);

        if (mr->halt)
        {
            break;
        }
        
        minirisc_decode_and_execute(mr);
        
        mr->PC = mr->next_PC;
    }
}

void minirisc_test() {
    platform_t* platformTest;
    minirisc_t* miniriscTest;

    platformTest = platform_new();
    miniriscTest = minirisc_new(0x80000000, platformTest);

    // Test de LUI
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lui_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Cela correspond a la valeur %u stocké dans le registre.\n",(int) miniriscTest->regs[1] >> 12);

    //TODO Faire la fonction AUIPC
    //Test de AUIPC
    // miniriscTest->halt = 0;
    // platform_load_program(platformTest, "");

    minirisc_free(miniriscTest);
    platform_free(platformTest);
}