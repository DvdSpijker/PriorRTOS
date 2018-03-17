/******************************************************************************************************************************************
 *  File: Convert.h
 *  Description: Standard conversions API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/


#ifndef CONVERT_H_
#define CONVERT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>

#define CONVERT_BUFFER_SIZE_RESULT_TO_STRING    15
#define CONVERT_BUFFER_SIZE_OSVERSION_TO_STRING 7


/******************************************************************************
 * @func: U32_t ConvertUsToMs(U32_t us)
 *
 * @desc: Convert microseconds (us) to milliseconds (ms).
 *
 * Arguments:
 * @argin: (U32_t) us; Value in microseconds.
 *
 * @rettype:  (U32_t) Value in milliseconds.
 * @retval:   0; Operation failed.
 * @retval:   Other; Valid value.
 ******************************************************************************/
U32_t ConvertUsToMs(U32_t us);

/******************************************************************************
 * @func: U32_t ConvertMsToUs(U32_t ms)
 *
 * @desc: Convert milliseconds (ms) to microseconds (us).
 *
 * Arguments:
 * @argin: (U32_t) ms; Value in milliseconds.
 *
 * @rettype:  (U32_t) Value in microseconds.
 * @retval:   0; Operation failed.
 * @retval:   Other; Valid value.
 ******************************************************************************/
U32_t ConvertMsToUs(U32_t ms);

/******************************************************************************
 * @func: U8_t ConvertResultToString(OsResult_t result,
 *              char *out_result_str)
 *
 * @desc: Converts a result of type OsResult_t to a null-terminated
 * string.
 *
 * Arguments:
 * @argin: (OsResult_t) result; Result to convert.
 * @argout: (char *) out_result_str; Array containing the result string. The
 *                                   array should be at least of size
 *                                   CONVERT_BUFFER_SIZE_RESULT_TO_STRING.
 *
 * @rettype:  (U8_t) number of characters (excluding \0).
 * @retval:   0; Operation failed.
 * @retval:   Other; Valid number of characters.
 ******************************************************************************/
U8_t ConvertResultToString(OsResult_t result, char *out_result_str);

/******************************************************************************
 * @func: U8_t ConvertIntToString(U32_t integer, char *out_int_str)
 *
 * @desc: Converts a integer value to a null-terminated string.
 *
 * Arguments:
 * @argin: (U32_t) integer; Integer value.
 * @argout: (char *) out_int_str; Array containing the integer string.
 *                                out_int_str[N-1] should be initialized at 0x20
 *                                (ASCII space) to prevent the conversion from
 *                                exceeding its bounds.
 *
 * @rettype:  (U8_t) number of characters (excluding \0).
 * @retval:   0; Operation failed.
 * @retval:   Other; Valid number of characters.
 ******************************************************************************/
U8_t ConvertIntToString(U32_t integer, char *out_int_str);

U32_t ConvertStringToInt(char *int_str);

U8_t ConvertIntToHexString(U32_t integer, char *out_hex_str);

U32_t ConvertHexStringToInt(char *hex_str);

/******************************************************************************
 * @func: U8_t ConvertIntToBytes(U32_t integer, U8_t num_bytes,
 *              U8_t *out_bytes)
 *
 * @desc: Convert an integer value into separate bytes. This allows
 * for easy splitting of for instance 32-bits value into 4 bytes.
 * The bytes will be stored with the LSB at 0.
 *
 * Arguments:
 * @argin: (U32_t) integer; Integer value.
 * @argin: (U8_t)  num_bytes; Number of bytes present in the integer value. 1-4.
 * @argout: (U8_t *) out_bytes; Array containing the bytes, should be at least of
 *                              size num_bytes.
 *                              out_bytes[0] = LSB. out_bytes[num_bytes-1] = MSB.
 *
 * @rettype:  (U8_t) result.
 * @retval:   0; Number of bytes has an illegal value i.e. 0 or >4.
 * @retval:   1; Operation successful.
 ******************************************************************************/
U8_t ConvertIntToBytes(U32_t integer, U8_t num_bytes, U8_t *out_bytes);

U32_t ConvertBytesToInt(U8_t *bytes, U8_t num_bytes);

char ConvertDecToChar(U8_t decimal);

U8_t ConvertCharToHex(char character);

U8_t ConvertCharToDec(char character);

/* Converts OS version to a null-terminated string of length 7.
 * Format: V<M>.<m>.<u> Major, minor, subversion. */
/******************************************************************************
 * @func: U8_t ConvertOsVersionToString(OsVer_t os_version,
 *              char *out_os_version_str)
 *
 * @desc: Converts OS version to a null-terminated string of length 7 in the following
 * format: V<M>.<m>.<svn> Major, minor, subversion.
 *
 * Arguments:
 * @argin: (OsVer_t) os_version; OS Version to convert.
 * @argout: (char *) out_os_version_str; Array containing the OS version string.
 *                                       The array should be at least of size
 *                                       CONVERT_BUFFER_SIZE_OS_VERSION_TO_STRING.
 *
 * @rettype:  (U8_t) number of characters (excluding \0).
 * @retval:   0; Operation failed.
 * @retval:   Other; Valid number of characters.
 ******************************************************************************/
U8_t ConvertOsVersionToString(OsVer_t os_version, char *out_os_version_str);

U8_t ConvertIdToString(Id_t id, char *out_id_str);

Id_t ConvertHexStringToId(char *hex_id_str);



#ifdef __cplusplus
}
#endif

#endif /* CONVERT_H_ */