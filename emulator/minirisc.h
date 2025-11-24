#ifndef MINIRISC_H
#define MINIRISC_H
#include <inttypes.h>

/**
 * Processor object.
 */
typedef struct {
	uint32_t    PC;       // Program counter register
	uint32_t    IR;       // Instruction register
	uint32_t    next_PC;  // Value used to update the PC after the exec stage
	uint32_t    regs[32]; // General purpose registers, r0 is hardwired to 0
	platform_t* platform; // The platform this core is connected to
	int         halt;     // Stop the emulator when other than 0
} minirisc_t;


/**
 * Allocate and initializes a new `minirisc_t` object.
 */
minirisc_t* minirisc_new(uint32_t initial_PC, platform_t *platform);

/**
 * Cleanup and free a `minirisc_t` object.
 */
void minirisc_free(minirisc_t *mr);

/**
 * Read the instruction pointed to by PC and place it in IR
 */
void minirisc_fetch(minirisc_t *mr);

/**
 * Decode the instruction in IR and execute it
 */
void minirisc_decode_and_execute(minirisc_t *mr);

/**
 * Run the processor:
 * minirisc_fetch() and minirisc_decode_and_execute()
 * in a loop while halt is false.
 */
void minirisc_run(minirisc_t *mr);
#endif