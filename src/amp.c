/**
 * @file amp.c
 * @brief Encoding and decoding of AMP messages
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

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
amp_msg_t *amp_create(void) {

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
 * @param data Data of the field
 * @param size Size of the field
 * @return 0 if the function succeeded, -1 otherwise
 */
int amp_push(amp_msg_t *amp, amp_type_e type, void *data, size_t size) {
  
  assert(NULL != amp);
  assert(NULL != data);

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

  /* Compute the length of the field */
  size_t len = (((AMP_TYPE_STRING == type) || (AMP_TYPE_JSON == type)) ? 1 : 0) + size;

  /* Fill the content of the field */
  if (NULL == (new_field->data = malloc(len))) {
    /* Unable to allocate memory */
    free(new_field);
    return -1;
  }
  memset(new_field->data, 0, len);
  memcpy(new_field->data, data, size);
  new_field->type = type;
  new_field->size = size;

  /* Add the field at the end of the message */
  if (NULL == amp->last) {
    amp->first = amp->last = new_field;
  } else {
    new_field->prev = amp->last;
    amp->last->next = new_field;
    amp->last = new_field;
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
int amp_encode(amp_msg_t *amp, void **buf, size_t *size) {
  
  assert(NULL != amp);
  assert(NULL != buf);
  assert(NULL != size);
  
  unsigned char *tmp1;
  size_t position = 0;
  
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
  tmp1[position] = (AMP_VERSION << 4) | (amp->count & 0x0F); position++;
  
  /* Add each field to the message */
  amp_field_t *curr_field = amp->first;
  while (NULL != curr_field) {

    /* Compute the length of the field */
    size_t len = ((AMP_TYPE_BLOB != curr_field->type) ? 2 : 0) + curr_field->size;
    
    /* Increase the size of the message to store the new field */
    void *tmp2 = realloc(tmp1, position + 4 + len);
    if (NULL == tmp2) {
      /* Unable to allocate memory */
      free(tmp1);
      return -1;
    }
    tmp1 = tmp2;
    
    /* Add size of the new field */
    tmp1[position] = (len & 0xFF000000) >> 24; position++;
    tmp1[position] = (len & 0x00FF0000) >> 16; position++;
    tmp1[position] = (len & 0x0000FF00) >> 8; position++;
    tmp1[position] = (len & 0x000000FF) >> 0; position++;
    
    /* Add type of the field */
    switch (curr_field->type) {
      case AMP_TYPE_BLOB:
        /* No type added to the message */
        break;
      case AMP_TYPE_STRING:
        tmp1[position] = 's'; position++;
        tmp1[position] = ':'; position++;
        break;
      case AMP_TYPE_BIGINT:
        tmp1[position] = 'b'; position++;
        tmp1[position] = ':'; position++;
        break;
      case AMP_TYPE_JSON:
        tmp1[position] = 'j'; position++;
        tmp1[position] = ':'; position++;
        break;
      default:
        /* Should not occur */
        break;
    }
    
    /* Copy field data */
    memcpy(&tmp1[position], curr_field->data, curr_field->size); position += curr_field->size;
    
    /* Next field */
    curr_field = curr_field->next;
  }
 
  /* Encoding completed */
  *buf = tmp1; *size = position;
 
  return 0;
}

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
int amp_decode(amp_msg_t *amp, void **buf, size_t *size) {

  assert(NULL != amp);
  assert(NULL != buf);
  assert(NULL != size);

  unsigned char *tmp1 = (unsigned char *)*buf;
  size_t position = 0;

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
      new_field->size = len - 2;
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
      if (NULL == (new_field->data = malloc(len - 2 + 1))) {
        /* Unable to allocate memory */
        free(new_field);
        return -1;
      }
      memcpy(new_field->data, &tmp1[position + 2], len - 2);
      ((unsigned char *)new_field->data)[len - 2] = '\0';
      new_field->size = len - 2;
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
      amp->last = new_field;
    }
  }
  
  /* Decoding completed */
  *buf += position; *size -= position;
 
  return 0;
}

/**
 * @brief Get number of fields in AMP message
 * @param amp AMP message
 * @return Number of fields if the function succeeded, -1 otherwise
 */
int amp_get_count(amp_msg_t *amp) {
  
  assert(NULL != amp);
 
  /* Return count value */
  return (int)amp->count;
}

/**
 * @brief Get first field in AMP message
 * @param amp AMP message
 * @return First field of the message if the function succeeded, NULL otherwise
 */
amp_field_t *amp_get_first(amp_msg_t *amp) {
  
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
amp_field_t *amp_get_next(amp_msg_t *amp) {
  
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
void amp_release(amp_msg_t *amp) {

  /* Release AMP message */
  if (NULL != amp) {
    amp_field_t *last_field = NULL;
    amp_field_t *curr_field = amp->first;
    while (NULL != curr_field) {
      last_field = curr_field;
      curr_field = curr_field->next;
      if (NULL != last_field->data) {
        free(last_field->data);
      }
      free(last_field);
    }
    free(amp);
  }
}
