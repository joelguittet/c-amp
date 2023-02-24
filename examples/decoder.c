/**
 * @file      decoder.c
 * @brief     AMP decoder example in C
 *
 * MIT License
 *
 * Copyright (c) 2021-2023 joelguittet and c-amp contributors
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

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <cJSON.h>

#include "amp.h"

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param argv Arguments
 * @return Always returns 0
 */
int
main(int argc, char **argv) {

    /* Create new AMP message */
    amp_msg_t *amp = amp_create();
    if (NULL == amp) {
        printf("unable to create amp message\n");
        exit(EXIT_FAILURE);
    }

    /* Create AMP message */
    unsigned char buffer[58] = { 0x14, 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x07, 0x73, 0x3a, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x00,
                                 0x00, 0x00, 0x0a, 0x62, 0x3a, 0xd9, 0xed, 0xfa, 0x3a, 0x47, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x6a, 0x3a, 0x7b,
                                 0x22, 0x70, 0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64, 0x22, 0x3a, 0x22, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x22, 0x7d };
    size_t        size       = 58;
    void *        buf        = malloc(size);
    if (NULL == buf) {
        printf("unable to malloc buffer\n");
        amp_release(amp);
        exit(EXIT_FAILURE);
    }
    memcpy(buf, buffer, size);
    void *tmp = buf; /* Copy the original buffer pointer because it will move in amp_decode */

    /* Decode AMP message */
    if (0 != amp_decode(amp, &tmp, &size)) {
        printf("unable to decode amp message\n");
        amp_release(amp);
        exit(EXIT_FAILURE);
    }
    free(buf);

    /* Parse all fields of the message */
    int64_t      bint;
    char *       str;
    amp_field_t *field = amp_get_first(amp);
    while (NULL != field) {

        /* Switch depending of the type */
        switch (field->type) {
            case AMP_TYPE_BLOB:
                printf("<Buffer");
                for (int index_data = 0; index_data < field->size; index_data++) {
                    printf(" %02x", ((unsigned char *)field->data)[index_data]);
                }
                printf(">\n");
                break;
            case AMP_TYPE_STRING:
                printf("%s\n", (char *)field->data);
                break;
            case AMP_TYPE_BIGINT:
                bint = (*(int64_t *)field->data);
                printf("%" PRId64 "\n", bint);
                break;
            case AMP_TYPE_JSON:
                str = cJSON_PrintUnformatted((cJSON *)field->data);
                printf("%s\n", str);
                free(str);
                break;
            default:
                /* Should not occur */
                break;
        }

        /* Next field */
        field = amp_get_next(amp);
    }

    /* Release memory */
    amp_release(amp);

    return 0;
}
