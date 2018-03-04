/**********************************************************************************************************************************************
*  File: Prior_periph.c
*
*  Description: Prior Peripheral HAL
*
*  OS Version: V0.4
*  Date: 13/03/2015
*
*  Author(s)
*  -----------------
*  D. van de Spijker
*  -----------------
*
*
*  Copyright© 2017    D. van de Spijker
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software AND associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights to use,
*  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
*  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
*  1. Redistributions of source code must retain the above copyright notice,
*  this list of conditions and the following disclaimer.
*
*  2. The name of Prior RTOS may not be used to endorse or promote products derived
*    from this Software without specific prior written permission.
*
*  3. This Software may only be redistributed and used in connection with a
*    product in which Prior RTOS is integrated. Prior RTOS shall not be
*    distributed, under a different name or otherwise, as a standalone product.
*
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
*  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
*  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/

#include <Peripheral.h>

#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
#include <RingBuffer.h>
#endif

OsResult_t PeripheralInit(struct PeripheralDescriptor *periph_desc)
{
	if(periph_desc == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->init == NULL) {
		return OS_RES_FAIL;
	}
	return periph_desc->init(periph_desc->hal_instance);
}

OsResult_t PeripheralOpen(struct PeripheralDescriptor *periph_desc)
{
	if(periph_desc == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->open == NULL) {
		return OS_RES_FAIL;
	}
	return periph_desc->open(periph_desc->hal_instance);	
}

OsResult_t PeripheralClose(struct PeripheralDescriptor *periph_desc)
{
	if(periph_desc == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->close == NULL) {
		return OS_RES_FAIL;
	}
	return periph_desc->close(periph_desc->hal_instance);	
}

OsResult_t PeripheralWriteSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *data)
{
	if(periph_desc == NULL || data == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->write_single == NULL) {
		return OS_RES_FAIL;
	}
	if(periph_desc->write_buffer == OS_ID_INVALID) {
		return periph_desc->write_single(periph_desc->hal_instance, data);	
	} else {
#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
		U32_t length = 1;
		return RingBufferWrite(periph_desc->write_buffer, data, &length, OS_TIMEOUT_INFINITE);
#else	
		return OS_RES_ERROR;
#endif
	}
}

OsResult_t PeripheralWriteBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *data, U32_t *size)
{
	if(periph_desc == NULL || data == NULL || size == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->write_block == NULL) {
		return OS_RES_FAIL;
	}
	if(*size == 0) {
		return OS_RES_INVALID_VALUE;
	}
	if(periph_desc->write_buffer == OS_ID_INVALID) {
		return periph_desc->write_block(periph_desc->hal_instance, data, size);	
	} else {
#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
		return RingBufferWrite(periph_desc->write_buffer, data, size, OS_TIMEOUT_INFINITE);
#else
		return OS_RES_ERROR;
#endif
	}
	
}

OsResult_t PeripheralReadSingle(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target)
{
	if(periph_desc == NULL || target == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->read_single == NULL) {
		return OS_RES_FAIL;
	}
	if(periph_desc->read_buffer == OS_ID_INVALID) {
		return periph_desc->read_single(periph_desc->hal_instance, target);
	} else {
#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
		U32_t length = 1;
		return RingBufferRead(periph_desc->read_buffer, target, &length, OS_TIMEOUT_INFINITE);
#else
		return OS_RES_ERROR;
#endif
	}
}

OsResult_t PeripheralReadBlock(struct PeripheralDescriptor *periph_desc, PeripheralBase_t *target, U32_t *size)
{
	if(periph_desc == NULL || target == NULL || size == NULL) {
		return OS_RES_NULL_POINTER;
	}
	if(periph_desc->read_block == NULL) {
		return OS_RES_FAIL;
	}
	if(*size == 0) {
		return OS_RES_INVALID_VALUE;
	}
	if(periph_desc->read_buffer == OS_ID_INVALID) {
		return periph_desc->read_block(periph_desc->hal_instance, target, size);
	} else {
#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
		return RingBufferRead(periph_desc->read_buffer, target, size, OS_TIMEOUT_INFINITE);
#else
		return OS_RES_ERROR;
#endif
	}	
}