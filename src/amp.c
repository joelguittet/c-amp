/**
 * @file      amp.c
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

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <cJSON.h>

#include "amp.h"

/******************************************************************************/
/* Definitions                                                                */
/******************************************************************************/

/* AMP protocol version */
#define AMP_VERSION 1

/* Maximum number of field in an AMP message */
#define AMP_MAX_FIELDS 15

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/**
 * @brief Function used to create an AMP message
 * @return AMP message if the function succeeded, NULL otherwise
 */
amp_msg_t *
amp_create(void) {

    /* Create new AMP handler */
    amp_msg_t *amp = (amp_msg_t *)malloc(sizeof(amp_msg_t));
    if (NULL == amp) {
        /* Unable to allocate message */
        return NULL;
    }
    memset(amp, 0, sizeof(amp_msg_t));

    return amp;
}

/**
 * @brief Push field to AMP message
 * @param amp AMP message
 * @param type Type of the field
 * @param ... data (and size for blob type) to be pushed
 * @return 0 if the function succeeded, -1 otherwise
 */
int
amp_push(amp_msg_t *amp, amp_type_e type, ...) {

    assert(NULL != amp);

    void   *blob = NULL;
    size_t  size = 0;
    char   *str  = NULL;
    int64_t bint = 0;
    cJSON  *json = NULL;

    /* Check number of fields in the message */
    if (AMP_MAX_FIELDS == amp->count) {
        /* Unable to add more fields */
        return -1;
    }

    /* Create new field */
    amp_field_t *new_field = (amp_field_t *)malloc(sizeof(amp_field_t));
    if (NULL == new_field) {
        /* Unable to allocate memory */
        return -1;
    }
    memset(new_field, 0, sizeof(amp_field_t));

    /* Fill the content of the field */
    va_list params;
    va_start(params, type);
    switch (type) {
        case AMP_TYPE_BLOB:
            blob = va_arg(params, void *);
            size = va_arg(params, int);
            va_end(params);
            if (NULL == (new_field->data = malloc(size))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(new_field->data, blob, size);
            new_field->type = type;
            new_field->size = size;
            break;
        case AMP_TYPE_STRING:
            str = va_arg(params, char *);
            va_end(params);
            if (NULL == (new_field->data = strdup(str))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            new_field->type = type;
            new_field->size = strlen(str);
            break;
        case AMP_TYPE_BIGINT:
            bint = va_arg(params, int64_t);
            va_end(params);
            if (NULL == (new_field->data = malloc(sizeof(int64_t)))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(new_field->data, &bint, sizeof(int64_t));
            new_field->type = type;
            new_field->size = sizeof(int64_t);
            break;
        case AMP_TYPE_JSON:
            json = va_arg(params, cJSON *);
            va_end(params);
            if (NULL == (new_field->data = cJSON_Duplicate(json, 1))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            new_field->type = type;
            new_field->size = sizeof(cJSON *);
            break;
        default:
            /* Should not occur */
            break;
    }

    /* Add the field at the end of the message */
    if (NULL == amp->last) {
        amp->first = amp->last = new_field;
    } else {
        new_field->prev = amp->last;
        amp->last->next = new_field;
        amp->last       = new_field;
    }

    /* Increase number of field in the message */
    amp->count++;

    return 0;
}

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
int
amp_encode(amp_msg_t *amp, void **buf, size_t *size) {

    assert(NULL != amp);
    assert(NULL != buf);
    assert(NULL != size);

    unsigned char *tmp1;
    size_t         position = 0;

    /* Check number of fields in the message */
    if (AMP_MAX_FIELDS < amp->count) {
        /* Unable to create this message */
        return -1;
    }

    /* Create AMP encoded buffer */
    if (NULL == (tmp1 = (unsigned char *)malloc(1))) {
        /* Unable to allocate message */
        return -1;
    }

    /* Fill header of the message */
    tmp1[position] = (AMP_VERSION << 4) | (amp->count & 0x0F);
    position++;

    /* Add each field to the message */
    amp_field_t *curr_field = amp->first;
    while (NULL != curr_field) {

        /* Compute the length of the field */
        size_t len             = 0;
        size_t curr_field_size = 0;
        void  *data            = NULL;
        switch (curr_field->type) {
            case AMP_TYPE_BLOB:
                data            = curr_field->data;
                curr_field_size = curr_field->size;
                len             = curr_field_size;
                break;
            case AMP_TYPE_STRING:
            case AMP_TYPE_BIGINT:
                data            = curr_field->data;
                curr_field_size = curr_field->size;
                len             = curr_field_size + 2;
                break;
            case AMP_TYPE_JSON:
                if (NULL == (data = (void *)cJSON_PrintUnformatted(curr_field->data))) {
                    /* Unable to allocate memory */
                    free(tmp1);
                    return -1;
                }
                curr_field_size = strlen((char *)data);
                len             = curr_field_size + 2;
                break;
            default:
                /* Should not occur */
                break;
        }

        /* Increase the size of the message to store the new field */
        void *tmp2 = realloc(tmp1, position + 4 + len);
        if (NULL == tmp2) {
            /* Unable to allocate memory */
            free(tmp1);
            return -1;
        }
        tmp1 = tmp2;

        /* Add size of the new field */
        tmp1[position] = (len & 0xFF000000) >> 24;
        position++;
        tmp1[position] = (len & 0x00FF0000) >> 16;
        position++;
        tmp1[position] = (len & 0x0000FF00) >> 8;
        position++;
        tmp1[position] = (len & 0x000000FF) >> 0;
        position++;

        /* Add type of the field */
        switch (curr_field->type) {
            case AMP_TYPE_BLOB:
                /* No type added to the message */
                break;
            case AMP_TYPE_STRING:
                tmp1[position] = 's';
                position++;
                tmp1[position] = ':';
                position++;
                break;
            case AMP_TYPE_BIGINT:
                tmp1[position] = 'b';
                position++;
                tmp1[position] = ':';
                position++;
                break;
            case AMP_TYPE_JSON:
                tmp1[position] = 'j';
                position++;
                tmp1[position] = ':';
                position++;
                break;
            default:
                /* Should not occur */
                break;
        }

        /* Copy field data */
        memcpy(&tmp1[position], data, curr_field_size);
        position += curr_field_size;

        /* Release memory if required */
        if (AMP_TYPE_JSON == curr_field->type) {
            free(data);
        }

        /* Next field */
        curr_field = curr_field->next;
    }

    /* Encoding completed */
    *buf  = tmp1;
    *size = position;

    return 0;
}

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
int
amp_decode(amp_msg_t *amp, void **buf, size_t *size) {

    assert(NULL != amp);
    assert(NULL != buf);
    assert(NULL != size);

    unsigned char *tmp1     = (unsigned char *)*buf;
    size_t         position = 0;

    /* Check protocol version */
    if (AMP_VERSION != ((tmp1[position] & 0xF0) >> 4)) {
        /* Unable to decode the message */
        return -1;
    }

    /* Retrieve number of fields in the message */
    amp->count = tmp1[position] & 0x0F;
    position++;

    /* Retrieve each field of the message */
    for (int index = 0; index < amp->count; index++) {

        /* Create field */
        amp_field_t *new_field = (amp_field_t *)malloc(sizeof(amp_field_t));
        if (NULL == new_field) {
            /* Unable to allocate memory */
            return -1;
        }
        memset(new_field, 0, sizeof(amp_field_t));

        /* Compute the length of the field */
        size_t len = (tmp1[position + 0] << 24) | (tmp1[position + 1] << 16) | (tmp1[position + 2] << 8) | (tmp1[position + 3] << 0);
        position += 4;

        /* Check field type */
        if (('s' == tmp1[position + 0]) && (':' == tmp1[position + 1])) {
            /* String */
            new_field->type = AMP_TYPE_STRING;
            if (NULL == (new_field->data = malloc(len - 2 + 1))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(new_field->data, &tmp1[position + 2], len - 2);
            ((unsigned char *)new_field->data)[len - 2] = '\0';
            new_field->size                             = len - 2;
        } else if (('b' == tmp1[position + 0]) && (':' == tmp1[position + 1])) {
            /* BigInt */
            new_field->type = AMP_TYPE_BIGINT;
            if (NULL == (new_field->data = malloc(sizeof(int64_t)))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(new_field->data, &tmp1[position + 2], sizeof(int64_t));
            new_field->size = sizeof(int64_t);
        } else if (('j' == tmp1[position + 0]) && (':' == tmp1[position + 1])) {
            /* Stringified JSON object */
            new_field->type = AMP_TYPE_JSON;
            void *data      = malloc(len - 2 + 1);
            if (NULL == data) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(data, &tmp1[position + 2], len - 2);
            ((unsigned char *)data)[len - 2] = '\0';
            if (NULL == (new_field->data = cJSON_Parse(data))) {
                /* Unable to allocate memory */
                free(new_field);
                free(data);
                return -1;
            }
            free(data);
            new_field->size = sizeof(cJSON *);
        } else {
            /* Blob */
            new_field->type = AMP_TYPE_BLOB;
            if (NULL == (new_field->data = malloc(len))) {
                /* Unable to allocate memory */
                free(new_field);
                return -1;
            }
            memcpy(new_field->data, &tmp1[position + 0], len);
            new_field->size = len;
        }
        position += len;

        /* Add the field at the end of the message */
        if (NULL == amp->last) {
            amp->first = amp->last = new_field;
        } else {
            new_field->prev = amp->last;
            amp->last->next = new_field;
            amp->last       = new_field;
        }
    }

    /* Decoding completed */
    *buf = (void *)(((unsigned char *)*buf) + position);
    *size -= position;

    return 0;
}

/**
 * @brief Get number of fields in AMP message
 * @param amp AMP message
 * @return Number of fields if the function succeeded, -1 otherwise
 */
int
amp_get_count(amp_msg_t *amp) {

    assert(NULL != amp);

    /* Return count value */
    return (int)amp->count;
}

/**
 * @brief Get first field in AMP message
 * @param amp AMP message
 * @return First field of the message if the function succeeded, NULL otherwise
 */
amp_field_t *
amp_get_first(amp_msg_t *amp) {

    assert(NULL != amp);

    /* Initialize curr field */
    amp->curr = amp->first;

    /* Return first field */
    return amp->curr;
}

/**
 * @brief Get next field in AMP message
 * @param amp AMP message
 * @return Next field of the message if the function succeeded, NULL otherwise
 */
amp_field_t *
amp_get_next(amp_msg_t *amp) {

    assert(NULL != amp);

    /* Next curr field */
    amp->curr = amp->curr->next;

    /* Return next field */
    return amp->curr;
}

/**
 * @brief Release AMP message
 * @param amp AMP message
 */
void
amp_release(amp_msg_t *amp) {

    /* Release AMP message */
    if (NULL != amp) {
        amp_field_t *last_field = NULL;
        amp_field_t *curr_field = amp->first;
        while (NULL != curr_field) {
            last_field = curr_field;
            curr_field = curr_field->next;
            if (NULL != last_field->data) {
                if (AMP_TYPE_JSON == last_field->type) {
                    cJSON_Delete(last_field->data);
                } else {
                    free(last_field->data);
                }
            }
            free(last_field);
        }
        free(amp);
    }
}
