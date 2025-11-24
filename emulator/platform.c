#include "platform.h"

platform_t* platform_new() {
    platform_t* platform;
    platform = (platform_t*) malloc(sizeof(platform_t));
    platform->memory = (uint32_t*) malloc(32*1024*1024*sizeof(uint8_t));
    return platform;
}

void platform_free(platform_t* platform) {
    free(platform->memory);
    free(platform);
}

int platform_read(platform_t *plt, access_type_t access_type, uint32_t addr, uint32_t *data) {
    if (addr == 0x10000000 || addr == 0x10000004 || addr == 0x10000008) {
        *data = 0;
        return 0;
    }

    if (addr < 0x80000000 || addr >= (0x80000000 + 0x2000000)) return -1; // On est hors champ

    uint32_t offset = addr - 0x80000000;
    switch (access_type)
    {
    case ACCESS_WORD :
        if (addr % 4 != 0)
            return -1;
        *data = plt->memory[offset/4];
        break;
    case ACCESS_HALF :
        if (addr % 2 != 0)
            return -1;
        *data = (uint32_t) *(int16_t*)((uint8_t*)plt->memory + offset); // On se deplace octets par octets. On change le type en pointeur d'un entier signé sur 16 bits qu'on déréférence. À l'issue on convertit finalement en un uint32. De cette maniere on conserve le signe et on a donc des 1 sur les bits rajoutés.
        break;
    case ACCESS_BYTE :
        *data = (uint32_t) *(int8_t*)((uint8_t*)plt->memory + offset);
        break;
    default:
        return -1;
        break;
    }

    return 0;

}

int platform_write(platform_t *plt, access_type_t access_type, uint32_t addr, uint32_t data) {
    
    switch (addr) {
        case 0x10000000:
            printf("%c", (char)data);
            return 0;
        case 0x10000004:
            printf("%d", (int32_t)data);
            return 0;
        case 0x10000008:
            printf("%x", data);
            return 0;
        default:
            break;
    }
    
    if (addr < 0x80000000 || addr >= (0x80000000 + 0x2000000)) return -1; // On est hors champ
    
    uint32_t offset = addr - 0x80000000;
    switch (access_type) {
    case ACCESS_WORD :
        if (addr % 4 != 0)
            return -1;
        plt->memory[offset/4] = data;
        break;
    case ACCESS_HALF :
        if (addr % 2 != 0)
            return -1;
        *((uint16_t*)((uint8_t*)plt->memory + offset)) = (uint16_t)data;
        break;
    case ACCESS_BYTE :
        *((uint8_t*)plt->memory + offset) = (uint8_t)data;
        break;
    default:
        break;
    }

    return 0;

}

void platform_load_program(platform_t *plt, const char *file_name) {
    FILE* program = fopen(file_name,"rb");

    // Permet de connaitre la taille du fichier en se mettant a la fin puis en revenant au debut avant de lire.
    fseek(program, 0, SEEK_END);
    long program_size = ftell(program);
    fseek(program, 0, SEEK_SET);

    fread(plt->memory,1,program_size,program);
    fclose(program);
}


// TODO Implémenter des tests unitaires