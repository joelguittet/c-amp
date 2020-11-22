/**
 * @file decoder.c
 * @brief AMP decoder example in C
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
int main(int argc, char** argv) {

  /* Create new AMP message */
  amp_msg_t *amp = amp_create();
  if (NULL == amp) {
    printf("unable to create amp message\n");
    exit(EXIT_FAILURE);
  }

  /* Decode AMP message */
  unsigned char buffer[58] = {0x14, 0x00, 0x00, 0x00, 0x03, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x07, 0x73, 0x3a, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x00, 0x00, 0x00, 0x0a, 0x62, 0x3a, 0xd9, 0xed, 0xfa, 0x3a, 0x47, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x6a, 0x3a, 0x7b, 0x22, 0x70, 0x61, 0x79, 0x6c, 0x6f, 0x61, 0x64, 0x22, 0x3a, 0x22, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x22, 0x7d}; size_t size = 58;
  void *buf = malloc(size); memcpy(buf, buffer, size);
  if (0 != amp_decode(amp, &buf, &size)) {
    printf("unable to decode amp message\n");
    amp_release(amp);
    exit(EXIT_FAILURE);
  }

  /* Parse all fields of the message */
  int index_data; int64_t bint;
  amp_field_t *field = amp_get_first(amp);
  while (NULL != field) {
  
    /* Switch depending of the type */
    switch (field->type) {
      case AMP_TYPE_BLOB:
        printf("<Buffer");
        for (index_data = 0; index_data < field->size; index_data++) {
          printf(" %02x", ((unsigned char *)field->data)[index_data]);
        }
        printf(">\n");
        break;
      case AMP_TYPE_STRING:
        printf("%s\n", (char *)field->data);
        break;
      case AMP_TYPE_BIGINT:
        bint = (int64_t)(*(int64_t *)field->data);
        printf("%ld\n", bint);
        break;
      case AMP_TYPE_JSON:
        printf("%s\n", (char *)field->data);
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
