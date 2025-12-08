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
    uint32_t newValue;
    uint32_t offset;
    int RD =  (mr->IR >> 7) & 0x1F; //Récupère le registre cible. On décale de 7 bits et on mets un masque pour avoir les 5 LSB de ce qu'il reste. 
    int RS = (mr->IR >> 12) & 0x1F;
    uint32_t MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
    int RS2 =  (mr->IR >> 7) & 0x1F; // Rennomage des RD et RS. Pour la clarté du code. 
    int RS1 = (mr->IR >> 12) & 0x1F;
    int RS2_R = (mr->IR >> 17) & 0x1F; //Registre RS2 pour les operations de type R


    switch (opcode) {
        case 1: // LUI
            newValue = (mr->IR & 0xFFFFF000) ;
            minirisc_set_reg(mr,RD,newValue);
            break;
        case 2: //AUIPC
            newValue = mr->PC + (mr->IR & 0xFFFFF000);
            minirisc_set_reg(mr,RD,newValue);
            break;
        case 3: //JAL
            offset = (mr->IR & 0xFFFFF000) >> 11; // LSB de l'offset a 0. Les MSBs a 0 avant d'étendre ou non pour le signe.
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFE00000;
            }
            mr->next_PC = mr->PC + offset;
            minirisc_set_reg(mr,RD,mr->PC + 4);
            break;
        case 4: //JALR
            offset = (mr->IR) >> 20;
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            mr->next_PC = (mr->regs[RS] + offset) & 0xFFFFFFFE;
            minirisc_set_reg(mr,RD,mr->PC + 4);
            break;
        case 5: // BEQ
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if (mr->regs[RS1] == mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 6: // BNE
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if (mr->regs[RS1] != mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 7: // BLT
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if ((int32_t) mr->regs[RS1] < (int32_t) mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 8: // BGE
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if ((int32_t) mr->regs[RS1] >= (int32_t) mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 9: // BLTU
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if (mr->regs[RS1] < mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 10: // BGEU
            offset = (mr->IR) >> 19; // Décalage
            offset &= 0xFFFFFFFE; //LSB a 0
            MSB_extensionDeSigne = (mr->IR >> 31); // Pour pouvoir étendre le signe
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFE000;
            }
            if (mr->regs[RS1] >= mr->regs[RS2]) {
                mr->next_PC = mr->PC + offset;
            }
            break;
        case 11: // LB A partir de la faut faire des tests unitaires TODO
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            platform_read((mr->platform),ACCESS_BYTE,mr->regs[RS] + offset, &newValue);
            minirisc_set_reg(mr,RD,newValue);
            break;
        case 12: // LH
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            if (platform_read(mr->platform, ACCESS_HALF, mr->regs[RS] + offset, &newValue) == 0) {
                minirisc_set_reg(mr, RD, newValue);
            }
            else {
                fprintf(stderr, "Erreur : Load address misaligned exception at 0x%08x\n", (mr->regs[RS] + offset));
                mr->halt = 1;
            }
            break;
        case 13: // LW
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            if (platform_read(mr->platform, ACCESS_WORD, mr->regs[RS] + offset, &newValue) == 0) {
                minirisc_set_reg(mr, RD, newValue);
            }
            else {
                fprintf(stderr, "Erreur : Load address misaligned exception at 0x%08x\n", (mr->regs[RS] + offset));
                mr->halt = 1;
            }
            break;
        case 14: // LBU
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            platform_read((mr->platform),ACCESS_BYTE,mr->regs[RS] + offset, &newValue);
            minirisc_set_reg(mr,RD,newValue & 0x000000FF);
            break;
        case 15: // LHU
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            if (platform_read(mr->platform, ACCESS_HALF, mr->regs[RS] + offset, &newValue) == 0) {
                minirisc_set_reg(mr, RD, newValue & 0x0000FFFF);
            }
            else {
                fprintf(stderr, "Erreur : Load address misaligned exception at 0x%08x\n", (mr->regs[RS] + offset));
                mr->halt = 1;
            }
            break;
        case 16: // SB
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            platform_write(mr->platform,ACCESS_BYTE,mr->regs[RS1] + offset, mr->regs[RS2]);
            break;
        case 17: // SH
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            if (platform_write(mr->platform, ACCESS_HALF, mr->regs[RS1] + offset, mr->regs[RS2]) != 0) {
                fprintf(stderr, "Erreur : Store address misaligned exception at 0x%08x\n", (mr->regs[RS] + offset));
                mr->halt = 1;
            }
            break;
        case 18: // SW
            offset = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                offset |= 0xFFFFF000;
            }
            if (platform_write(mr->platform, ACCESS_WORD, mr->regs[RS1] + offset, mr->regs[RS2]) != 0) {
                fprintf(stderr, "Erreur : Store address misaligned exception at 0x%08x\n", (mr->regs[RS] + offset));
                mr->halt = 1;
            }
            break;
        case 19: // ADDI
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            minirisc_set_reg(mr,RD,mr->regs[RS] + newValue);
            break;
        case 20: // SLTI
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            if ((int32_t) mr->regs[RS] < (int32_t) newValue) {
                minirisc_set_reg(mr,RD,0x1);
            }
            else {
                minirisc_set_reg(mr,RD,0x0);
            }
            break;
        case 21: // SLTIU
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            if (mr->regs[RS] < newValue) {
                minirisc_set_reg(mr,RD,0x1);
            }
            else {
                minirisc_set_reg(mr,RD,0x0);
            }
            break;
        case 22: // XORI
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            minirisc_set_reg(mr,RD,(mr->regs[RS]) ^ newValue);
            break;
        case 23: //ORI
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            minirisc_set_reg(mr,RD,(mr->regs[RS]) | newValue);
            break;
        case 24: // ANDI
            newValue = (mr->IR) >> 20;
            if (MSB_extensionDeSigne == 0x1) {
                newValue |= 0xFFFFF000;
            }
            minirisc_set_reg(mr,RD,(mr->regs[RS]) & newValue);
            break;
        case 25: // SLLI
            newValue = (mr->IR) >> 20;
            newValue &= 0x001F;
            minirisc_set_reg(mr,RD,mr->regs[RS] << newValue);
            break;
        case 26: // SRLI
            newValue = (mr->IR) >> 20;
            newValue &= 0x001F;
            minirisc_set_reg(mr,RD,mr->regs[RS] >> newValue);
            break;
        case 27: // SRAI
            newValue = (mr->IR) >> 20;
            newValue &= 0x001F;            
            minirisc_set_reg(mr,RD,((int32_t) mr->regs[RS]) >> newValue);
            break;
        case 28: // ADD
            minirisc_set_reg(mr,RD, mr->regs[RS1] + mr->regs[RS2_R]);
            break;
        case 29: // SUB
            minirisc_set_reg(mr,RD, mr->regs[RS1] - mr->regs[RS2_R]);
            break;
        case 30: // SLL
            minirisc_set_reg(mr,RD,mr->regs[RS1] << (mr->regs[RS2_R] & 0x1F));
            break;
        case 31: // SRL
            minirisc_set_reg(mr,RD,mr->regs[RS1] >> (mr->regs[RS2_R] & 0x1F));
            break;
        case 32: // SRA
            minirisc_set_reg(mr,RD,((int32_t)(mr->regs[RS1])) >> (mr->regs[RS2_R] & 0x1F));
            break;
        case 33: // SLT
            if ((int32_t) mr->regs[RS1] < (int32_t) mr->regs[RS2_R]) {
                minirisc_set_reg(mr,RD,0x1);
            }
            else {
                minirisc_set_reg(mr,RD,0x0);
            }
            break;
        case 34: // SLTU
            if (mr->regs[RS1] < mr->regs[RS2_R]) {
                minirisc_set_reg(mr,RD,0x1);
            }
            else {
                minirisc_set_reg(mr,RD,0x0);
            }
            break;
        case 35: //XOR TODO on continue les instructions. On fera les tests a la fin.
            //TODO
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
    printf("=== Test pour LUI ===\n");
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lui_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Cela correspond a une valeur immédiate de %u pour lui.\n",(int) miniriscTest->regs[1] >> 12);

    // Test de AUIPC
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    printf("=== Test pour AUIPC ===\n");
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/auipc_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Cela correspond a une valeur immédiate de %u pour auipc.\n",(int) ((miniriscTest->regs[1] - 0x80000000) >> 12));

    // Test de JAL
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/jal_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour JAL ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de JALR
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/jalr_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour JALR ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Le nextPC a la fin de l'éxécution est: %x\n",miniriscTest->next_PC);

    // Test de BEQ
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/beq_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BEQ ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);

    // Test de BNE
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/bne_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BNE ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);

    // Test de BLT
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/blt_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BLT ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);

    // Test de BGE
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/bge_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BGE ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);

    // Test de BLTU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/bltu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BLTU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de BGEU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/bltu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour BGEU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);


    minirisc_free(miniriscTest);
    platform_free(platformTest);
}