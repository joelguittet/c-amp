/**
 * @file encoder.c
 * @brief AMP encoder example in C
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

  /* Push blob to AMP message */
  unsigned char tmp[3] = {1, 2, 3};
  amp_push(amp, AMP_TYPE_BLOB, tmp, 3);

  /* Push string to AMP message */
  char hello[] = "hello";
  amp_push(amp, AMP_TYPE_STRING, hello, strlen(hello));

  /* Push BigInt to AMP message */
  int64_t bint = 123451234512345;
  amp_push(amp, AMP_TYPE_BIGINT, &bint, sizeof(int64_t));

  /* Push stringified JSON to AMP message */
  char json[] = "{\"payload\":\"value\"}";
  amp_push(amp, AMP_TYPE_JSON, json, strlen(json));
  
  /* Encode AMP message */
  void *buffer = NULL; size_t size = 0;
  if (0 != amp_encode(amp, &buffer, &size)) {
    printf("unable to encode amp message\n");
    amp_release(amp);
    exit(EXIT_FAILURE);
  }
  
  /* Display AMP buffer */
  printf("encoded buffer size=%d, content='", (int)size);
  for (size_t index = 0; index < size; index++) {
    printf("0x%02x%s", ((unsigned char *)buffer)[index], (index < size - 1) ? ", " : "");
  }
  printf("'\n");
  
  /* Release memory */
  amp_release(amp);

  return 0;
}
