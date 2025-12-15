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
    minirisc->csr.mstatus = 0;
    minirisc->csr.mepc = 0;

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

uint32_t csr_read(minirisc_t *mr, uint32_t csr_num) {
    switch (csr_num) {
        case 0x300: return mr->csr.mstatus;
        case 0x341: return mr->csr.mepc;
        default: return 0; // CSR inconnu ou non implémenté
    }
}

void csr_write(minirisc_t *mr, uint32_t csr_num, uint32_t value) {
    switch (csr_num) {
        case 0x300: mr->csr.mstatus = value; break;
        case 0x341: mr->csr.mepc = value; break;
        default: break; // On ignore les écritures vers des CSR inconnus
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

    uint32_t csrnum = (mr->IR) >> 20;
    uint32_t old_val;
    int imm5 = (mr->IR >> 12) & 0x1F;



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
        case 11: // LB
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
        case 35: // XOR
            minirisc_set_reg(mr,RD,mr->regs[RS1] ^ mr->regs[RS2_R]);
            break;
        case 36: // OR
            minirisc_set_reg(mr,RD,mr->regs[RS1] | mr->regs[RS2_R]);
            break;
        case 37: // AND
            minirisc_set_reg(mr,RD,mr->regs[RS1] & mr->regs[RS2_R]);
            break;
        case 38: // ECALL
            minirisc_set_reg(mr,10,-1);
            break;
        case 39: // EBREAK
            mr->halt = 1;
            break;
        case 40: // RETI
            mr->csr.mstatus |= 0x2; // On met a jour le bit INTERRUPT_ENABLE.
            mr->next_PC = mr->csr.mepc;
            break;
        case 41: // WFI
            break;
        case 42: // CSRRW
            old_val = mr->regs[RS];
            if (RD != 0) {
                minirisc_set_reg(mr,RD,csr_read(mr,csrnum));
            }
            csr_write(mr,csrnum,old_val);
            break;
        case 43: // CSRRS
            old_val = csr_read(mr,csrnum);
            if (RD != 0) {
                csr_write(mr,csrnum, csr_read(mr, csrnum) | mr->regs[RS]);
            }
            minirisc_set_reg(mr,RD,csr_read(mr, csrnum));
            break;
        case 44: // CSRRC
            old_val = csr_read(mr,csrnum);
            if (RD != 0) {
                csr_write(mr,csrnum, csr_read(mr, csrnum) & ~mr->regs[RS]);
            }
            minirisc_set_reg(mr,RD,csr_read(mr, csrnum));
            break;
        case 45: // CSRRWI
            if (RD != 0) {
                minirisc_set_reg(mr, RD, csr_read(mr,csrnum));
            }
            csr_write(mr, csrnum, imm5);
            break;
        case 46: // CSRRSI
            minirisc_set_reg(mr, RD, csr_read(mr,csrnum));
            if (imm5 != 0) {
                csr_write(mr, csrnum, csr_read(mr, csrnum) | imm5);
            }
            break;
        case 47: // CSRRCI
            minirisc_set_reg(mr, RD, csr_read(mr,csrnum));
            if (imm5 != 0) {
                csr_write(mr, csrnum, csr_read(mr, csrnum) & ~imm5);
            }
            break;
        case 56: // MUL
            minirisc_set_reg(mr,RD, mr->regs[RS1] * mr->regs[RS2_R]);
            break;
        case 57: // MULH
            minirisc_set_reg(mr, RD, (uint32_t)(((int64_t)(int32_t)mr->regs[RS1] * (int64_t)(int32_t)mr->regs[RS2_R]) >> 32));
            break;
        case 58: // MULHSU
            minirisc_set_reg(mr, RD, (uint32_t)(((int64_t)(int32_t)mr->regs[RS1] * (uint64_t)mr->regs[RS2_R]) >> 32));
            break;
        case 59: // MULHU
            minirisc_set_reg(mr, RD, (uint32_t)(((uint64_t)mr->regs[RS1] * (uint64_t)mr->regs[RS2_R]) >> 32));
            break;
        case 60: // DIV
            if ((int32_t)mr->regs[RS2_R] == 0) {
                minirisc_set_reg(mr,RD,-1);
            }
            else if (mr->regs[RS1] == 0x80000000 && (int32_t)mr->regs[RS2_R] == -1) {
                minirisc_set_reg(mr,RD,mr->regs[RS1]);
            }
            else {
                minirisc_set_reg(mr,RD,(int32_t)mr->regs[RS1] / (int32_t)mr->regs[RS2_R]);
            }
            break;
        case 61: //DIVU
            if (mr->regs[RS2_R] == 0) {
                minirisc_set_reg(mr, RD, 0xFFFFFFFF);
            }
            else {
                minirisc_set_reg(mr, RD, mr->regs[RS1] / mr->regs[RS2_R]);
            }
            break;
        case 62: // REM
            if ((int32_t)mr->regs[RS2_R] == 0) {
                minirisc_set_reg(mr, RD, (int32_t)mr->regs[RS1]);
            }
            else if ((int32_t)mr->regs[RS1] == -2147483648 && (int32_t)mr->regs[RS2_R] == -1) {
                minirisc_set_reg(mr, RD, 0);
            }
            else {
                minirisc_set_reg(mr, RD, (int32_t)mr->regs[RS1] % (int32_t)mr->regs[RS2_R]);
            }
            break;
        case 63: // REMU
            if (mr->regs[RS2_R] == 0) {
                minirisc_set_reg(mr, RD, mr->regs[RS1]);
            }
            else {
                minirisc_set_reg(mr, RD, mr->regs[RS1] % mr->regs[RS2_R]);
            }
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
    printf("=== Test x0 ===\n");
    minirisc_set_reg(miniriscTest, 0, 0xDEADBEEF);
    
    if (miniriscTest->regs[0] == 0) {
        printf("SUCCES : x0 est bien reste a 0.\n");
    } else {
        printf("ECHEC : x0 a ete modifie (valeur : %x)\n", miniriscTest->regs[0]);
    }

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

    // Test de LB
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lb_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour LB ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de LH
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lh_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour LH ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de LW
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lw_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour LW ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de LBU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lbu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour LBU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de LHU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/lhu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour LHU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de SB
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sb_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SB ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SH
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sh_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SH ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SW
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sw_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SW ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de ADDI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/addi_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour ADDI ===\n");
    printf("Valeur dans le registre 1 en hexadecimal: %x\n",miniriscTest->regs[1]);
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de SLTI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/slti_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLTI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de SLTIU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sltiu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLTIU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de XORI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/xori_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour XORI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de ORI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/ori_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour ORI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de ANDI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/andi_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour ANDI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SLLI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/slli_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLLI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SRLI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/srli_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SRLI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);

    // Test de SRAI
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/srai_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SRAI ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de ADD
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/add_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour ADD ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SUB
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sub_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SUB ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de SLL
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sll_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLL ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de SRL
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/srl_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SRL ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de SRA
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sra_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SRA ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de SLT
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/slt_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLT ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);
    printf("Valeur dans le registre 6 en hexadecimal: %x\n",miniriscTest->regs[6]);

    // Test de SLTU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/sltu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour SLTU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de XOR
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/xor_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour XOR ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 7 en hexadecimal: %x\n",miniriscTest->regs[7]);

    // Test de OR
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/or_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour OR ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);

    // Test de AND
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/and_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour AND ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 6 en hexadecimal: %x\n",miniriscTest->regs[6]);
    printf("Valeur dans le registre 9 en hexadecimal: %x\n",miniriscTest->regs[9]);

    // ----------------------------------------------------------------------------------

    // Test de MUL
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/mul_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour MUL ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    // Test de MULH
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/mulh_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour MULH ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 7 en hexadecimal: %x\n",miniriscTest->regs[7]);

    // Test de MULHSU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/mulhsu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour MULHSU ===\n");
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de MULHU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/mulhu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour MULHU ===\n");
    printf("Valeur dans le registre 2 en hexadecimal: %x\n",miniriscTest->regs[2]);
    printf("Valeur dans le registre 3 en hexadecimal: %x\n",miniriscTest->regs[3]);

    // Test de DIV
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/div_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour DIV ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);
    printf("Valeur dans le registre 8 en hexadecimal: %x\n",miniriscTest->regs[8]);

    // Test de DIVU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/divu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour DIVU ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);
    printf("Valeur dans le registre 6 en hexadecimal: %x\n",miniriscTest->regs[6]);

    // Test de REM
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/rem_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour REM ===\n");
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);
    printf("Valeur dans le registre 6 en hexadecimal: %x\n",miniriscTest->regs[6]);
    printf("Valeur dans le registre 7 en hexadecimal: %x\n",miniriscTest->regs[7]);

    // Test de REMU
    miniriscTest->halt = 0;
    miniriscTest->PC = 0x80000000;
    platform_load_program(platformTest, "/home/wizhack/Document/ensta/architectureordinateurs/embedded_software/remu_test/build/esw.bin");
    minirisc_run(miniriscTest);
    printf("=== Test pour REMU ===\n");
    printf("Valeur dans le registre 4 en hexadecimal: %x\n",miniriscTest->regs[4]);
    printf("Valeur dans le registre 5 en hexadecimal: %x\n",miniriscTest->regs[5]);

    minirisc_free(miniriscTest);
    platform_free(platformTest);
}