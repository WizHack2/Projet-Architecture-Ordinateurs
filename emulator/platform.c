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

void platform_read(platform_t *plt, access_type_t access_type, uint32_t addr, uint32_t *data) {
    if (addr == 0x10000000 || addr == 0x10000004 || addr == 0x10000008) return 0;
    if (addr < 0x80000000 || addr >= (0x80000000 + 0x2000000)) return -1; // On est hors champ
    uint32_t offset = addr - 0x80000000;
    *data = plt->memory[offset]; // TODO Cf chatgpt pour continuer plus tard et comprendre WTF

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