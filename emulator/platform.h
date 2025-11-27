#ifndef PLATFORM_H
#define PLATFORM_H
#include <inttypes.h>
#include <stdio.h>

typedef struct {
    uint32_t *memory;
} platform_t;

/**
 * Type of memory acess
 */
typedef enum {
    ACCESS_BYTE = 0, //  8 bits
    ACCESS_HALF = 1, // 16 bits
    ACCESS_WORD = 3  // 32 bits
} access_type_t;

/** 
 * Allocates an initializes a new platform and its memory.
 */
platform_t* platform_new();

/**
 * Cleanup the platform's allocated memories.
 */
void platform_free(platform_t *platform);

/**
 * Read one item from the platform.
 * @param platform The platform object
 * @param type     Width of the access
 * @param addr     Address where to read the item
 * @param data     The item is placed in data.
 * @return         0 on success, -1 on error (illegal or misaligned access)
 */
int platform_read(platform_t *plt, access_type_t access_type, uint32_t addr, uint32_t *data);

/**
 * Write one item to the platform.
 * @param platform The platform object
 * @param type     Width of the access
 * @param addr     Address where to write the item
 * @param data     The item to write
 * @return         0 on success, -1 on error (illegal or misaligned access)
 */
int platform_write(platform_t *plt, access_type_t access_type, uint32_t addr, uint32_t data);

/**
 * Read the file named file_name and write its content
 * in the platform's memory.
 */
void platform_load_program(platform_t *plt, const char *file_name);

/**
 * Lance un test pour voir le bon fonctionnement de platform.c
 */
void platform_test();
#endif