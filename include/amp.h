/**
 * @file      amp.h
 * @brief     Encoding and decoding of AMP messages
 *
 * MIT License
 *
 * Copyright joelguittet and c-amp contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __AMP_H__
#define __AMP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

AMP_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
AMP_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
AMP_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the AMP_API_VISIBILITY flag to "export" the same symbols the way AMP_EXPORT_SYMBOLS does

*/

#define AMP_CDECL   __cdecl
#define AMP_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(AMP_HIDE_SYMBOLS) && !defined(AMP_IMPORT_SYMBOLS) && !defined(AMP_EXPORT_SYMBOLS)
#define AMP_EXPORT_SYMBOLS
#endif

#if defined(AMP_HIDE_SYMBOLS)
#define AMP_PUBLIC(type) type AMP_STDCALL
#elif defined(AMP_EXPORT_SYMBOLS)
#define AMP_PUBLIC(type) __declspec(dllexport) type AMP_STDCALL
#elif defined(AMP_IMPORT_SYMBOLS)
#define AMP_PUBLIC(type) __declspec(dllimport) type AMP_STDCALL
#endif
#else /* !__WINDOWS__ */
#define AMP_CDECL
#define AMP_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined(__SUNPRO_C)) && defined(AMP_API_VISIBILITY)
#define AMP_PUBLIC(type) __attribute__((visibility("default"))) type
#else
#define AMP_PUBLIC(type) type
#endif
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/* Definitions                                                                */
/******************************************************************************/

/* AMP types */
typedef enum {
    AMP_TYPE_BLOB,   /* Blob buffer */
    AMP_TYPE_STRING, /* String */
    AMP_TYPE_BIGINT, /* BigInt */
    AMP_TYPE_JSON    /* Stringified JSON object */
} amp_type_e;

/* AMP field */
typedef struct amp_field_s {
    struct amp_field_s *prev; /* Previous field of the message*/
    struct amp_field_s *next; /* Next field of the message*/
    amp_type_e          type; /* Type of the field */
    void               *data; /* Data of the field */
    size_t              size; /* Size of the field */
} amp_field_t;

/* AMP message */
typedef struct {
    amp_field_t *first; /* Daisy chain of fields in the AMP message */
    amp_field_t *last;  /* Point to the last field in the AMP message - Used to push fields in the message */
    amp_field_t *curr;  /* Point to the currently read field of the AMP message - Used to pull fields from the message */
    size_t       count; /* Number of fields in the AMP message */
} amp_msg_t;

/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Function used to create an AMP message
 * @return AMP message if the function succeeded, NULL otherwise
 */
AMP_PUBLIC(amp_msg_t *) amp_create(void);

/**
 * @brief Push field to AMP message
 * @param amp AMP message
 * @param type Type of the field
 * @param ... data (and size for blob type) to be pushed
 * @return 0 if the function succeeded, -1 otherwise
 */
AMP_PUBLIC(int) amp_push(amp_msg_t *amp, amp_type_e type, ...);

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
AMP_PUBLIC(int) amp_encode(amp_msg_t *amp, void **buf, size_t *size);

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
AMP_PUBLIC(int) amp_decode(amp_msg_t *amp, void **buf, size_t *size);

/**
 * @brief Get number of fields in AMP message
 * @param amp AMP message
 * @return Number of fields if the function succeeded, -1 otherwise
 */
AMP_PUBLIC(int) amp_get_count(amp_msg_t *amp);

/**
 * @brief Get first field in AMP message
 * @param amp AMP message
 * @return First field of the message if the function succeeded, NULL otherwise
 */
AMP_PUBLIC(amp_field_t *) amp_get_first(amp_msg_t *amp);

/**
 * @brief Get next field in AMP message
 * @param amp AMP message
 * @return Next field of the message if the function succeeded, NULL otherwise
 */
AMP_PUBLIC(amp_field_t *) amp_get_next(amp_msg_t *amp);

/**
 * @brief Release AMP message
 * @param amp AMP message
 */
AMP_PUBLIC(void) amp_release(amp_msg_t *amp);

#ifdef __cplusplus
}
#endif

#endif /* __AMP_H__ */
