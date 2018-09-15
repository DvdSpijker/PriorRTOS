/*
 * Crc32.h
 *
 *  Created on: 10 jul. 2018
 *      Author: Dorus
 */

#ifndef CRC32_H_
#define CRC32_H_

#include "include/StdTypes.h"

U32_t Crc32 (U32_t prev_crc, const void *buf, U32_t len);

#endif /* CRC32_H_ */
