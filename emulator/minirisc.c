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

void minirisc_set_reg(minirisc_t* mr, int n_registre, uint32_t valeur) {
    if (n_registre != 0) mr->regs[n_registre] = valeur;
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
    int RD =  (mr->IR >> 7) & 0x1F; //Récupère le registre cible. On décale de 7 bits et on mets un masque pour avoir les 5 LSB de ce qu'il reste. 

    switch (opcode) {
        case 1: // LUI
            uint32_t newValue = (mr->IR & 0xFFFFF000) ;
            minirisc_set_reg(mr,RD,newValue);
            break;
        case 2: //AUIPC
            uint32_t newValue = mr->PC + (mr->IR & 0xFFFFF000);
            minirisc_set_reg(mr,RD,newValue);
            break;
        case 3: //JAL
            uint32_t offset = (mr->IR & 0x001FFFFE); // LSB de l'offset a 0. Les MSBs a 0 avant d'étendre ou non pour le signe.
            uint32_t MSB_extensionDeSigne = (mr->IR & 0x00100000); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x00100000) {
                offset += 0xFFFE0000;
            }
            mr->PC += offset;
            minirisc_set_reg(mr,RD,mr->PC + 4);
            break;
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

    // Test écriture sur le registre x0
    // TODOs

    // Test de LUI
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lui_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Cela correspond a une valeur immédiate de %u pour lui.\n",(int) miniriscTest->regs[1] >> 12);

    // Test de AUIPC
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/auipc_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Cela correspond a une valeur immédiate de %u pour auipc.\n",(int) ((miniriscTest->regs[1] - 0x80000000) >> 12));

    //TODO Faire tests unitaires pour JAL.

    minirisc_free(miniriscTest);
    platform_free(platformTest);
}