/*
* Convert.c
*
* Created: 4-3-2017 14:04:05
*  Author: Dorus
*/

#include <Convert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

U8_t ConvertResultToString(OsResult_t result, char *out_result_str)
{
    uint8_t strlen;

    switch (result) {
        case OS_RES_OK:
        strlen = 9;
        strncpy(out_result_str, "OS_RES_OK", strlen);
        break;

        case OS_RES_ERROR:
        strlen = 12;
        strncpy(out_result_str, "OS_RES_ERROR", strlen);
        break;

        case OS_RES_CRIT_ERROR:
        strlen = 15;
        strncpy(out_result_str, "OS_RES_CRIT_ERR", strlen);
        break;

        case OS_RES_FAIL:
        strlen = 11;
        strncpy(out_result_str, "OS_RES_FAIL", strlen);
        break;

        case OS_RES_NULL_POINTER:
        strlen = 16;
        strncpy(out_result_str, "OS_RES_NULL_PTR", strlen);
        break;

        case OS_RES_OUT_OF_BOUNDS:
        strlen = 18;
        strncpy(out_result_str, "OS_RES_OUT_OF_BOUNDS", strlen);
        break;

        case OS_RES_LOCKED:
        strlen = 13;
        strncpy(out_result_str, "OS_RES_LOCKED", strlen);
        break;

        default:
        strlen = 11;
        strncpy(out_result_str, "INVALID", strlen);
        break;
    }

    out_result_str[strlen] = '\0';

    return strlen;
}

U32_t ConvertUsToMs(U32_t us)
{

    return (U32_t)(us / 1e3);
}

U32_t ConvertMsToUs(U32_t ms)
{
    return ((U32_t) ms * 1e3);
}


char ConvertDecToChar(U8_t decimal)
{
    return ((decimal <= 9) ? (decimal + 48) : 0);
}

U8_t ConvertCharToDec(char character)
{
    return (U8_t)(character - 48);
}

U8_t ConvertCharToHex(char character)
{
    return 0;
}

U8_t ConvertOsVersionToString(OsVer_t os_version, char *out_os_version_str)
{
    U8_t len = 0;
    U8_t os_version_parts[3];
    if(ConvertIntToBytes((U32_t)os_version, 2, os_version_parts) == 1) {
        os_version_parts[2] = os_version_parts[1];
        os_version_parts[1] = os_version_parts[0] >> 4;
        os_version_parts[0] &= 0x0F;
        len = 6;
        out_os_version_str[0] = 'V';
        out_os_version_str[1] = ConvertDecToChar(os_version_parts[2]);
        out_os_version_str[2] = '.';
        out_os_version_str[3] = ConvertDecToChar(os_version_parts[1]);
        out_os_version_str[4] = '.';
        out_os_version_str[5] = ConvertDecToChar(os_version_parts[0]);
        out_os_version_str[6] = '\0';
    }

    return len;
}

U8_t ConvertIntToBytes(U32_t integer, U8_t num_bytes, U8_t *out_bytes)
{
    if(num_bytes == 0 || num_bytes > 4) {
        return 0;
    }
    out_bytes[0] = (U8_t)(integer & 0x000000FF);

    if(num_bytes > 1) {
        out_bytes[1] = (U8_t)((integer & 0x0000FF00) >> 8);
    }

    if(num_bytes > 2) {
        out_bytes[2] = (U8_t)((integer & 0x00FF0000) >> 16);
    }

    if(num_bytes > 3) {
        out_bytes[3] = (U8_t)((integer & 0xFF000000) >> 24);
    }

    return 1;
}

U32_t ConvertBytesToInt(U8_t *bytes, U8_t num_bytes)
{
    U32_t integer = 0;
    
    if(num_bytes == 0 || num_bytes > 4) {
        return 0;
    }
    
    integer |= (bytes[0] << 24);
    
    if(num_bytes > 1) {
        integer |= (bytes[1] << 16);
    }
    
    if(num_bytes > 2) {
        integer |= (bytes[2] << 8);
    }
    
    if(num_bytes > 3) {
        integer |= (bytes[3]);
    }

    return integer;
}

U8_t ConvertIntToString(U32_t integer, char *out_int_str)
{
    U32_t divisor = 1;

    /* Figure out initial divisor */
    while (integer / divisor > 9) {
        divisor *= 10;
    }

    U8_t i = 0;
    bool last_buffer_index = false;
    /* Now mod and print, then divide divisor.
    * While keeping in mind the bounds of the
    * string buffer. */
    do {
        if(out_int_str[i] == 0x20) {
            last_buffer_index = true;
        }
        out_int_str[i] =((char)('0' + (integer / divisor % 10)));
        divisor /= 10;
        i++;
    } while (divisor > 0 && i < 254 && last_buffer_index == false);

    return i;
}


Id_t ConvertHexStringToId(char *hex_id_str)
{
	Id_t id = ID_INVALID;
	sscanf(hex_id_str, "%08x", (unsigned int *)&id);
    return id;
}
