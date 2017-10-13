#ifndef FLETCHER_H_
#define FLETCHER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <Types.h>

/* Generates a 16-bits Fletcher checksum from given data.
 * Returns 0 if an error has occurred. */
U16_t FletcherGenerate(U8_t *data, U16_t dlen);

/* Verifies whether the data is corrupt using the Fletcher16 checksum.
 * Returns true if the checksum is correct. */
bool FletcherVerify(U16_t fletcher, U8_t *data, U16_t dlen);

#ifdef __cplusplus
}
#endif
#endif