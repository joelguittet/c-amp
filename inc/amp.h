/**
 * @file amp.h
 * @brief Encoding and decoding of AMP messages
 */

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/

#include <stdint.h>


/******************************************************************************/
/* Definitions                                                                */
/******************************************************************************/

/* AMP types */
typedef enum {
  AMP_TYPE_BLOB,                                                    /* Blob buffer */
  AMP_TYPE_STRING,                                                  /* String */
  AMP_TYPE_BIGINT,                                                  /* BigInt */
  AMP_TYPE_JSON                                                     /* Stringified JSON object */
} amp_type_e;

/* AMP field */
typedef struct amp_field_s {
  struct amp_field_s *prev;                                         /* Previous field of the message*/
  struct amp_field_s *next;                                         /* Next field of the message*/
  amp_type_e type;                                                  /* Type of the field */
  void *data;                                                       /* Data of the field */
  size_t size;                                                      /* Size of the field */
} amp_field_t;

/* AMP message */
typedef struct {
  amp_field_t *first;                                               /* Daisy chain of fields in the AMP message */
  amp_field_t *last;                                                /* Point to the last field in the AMP message - Used to push fields in the message */
  amp_field_t *curr;                                                /* Point to the currently read field of the AMP message - Used to pull fields from the message */
  size_t count;                                                     /* Number of fields in the AMP message */
} amp_msg_t;


/******************************************************************************/
/* Prototypes                                                                 */
/******************************************************************************/

/**
 * @brief Function used to create an AMP message
 * @return AMP message if the function succeeded, NULL otherwise
 */
amp_msg_t *amp_create(void);

/**
 * @brief Push field to AMP message
 * @param amp AMP message
 * @param type Type of the field
 * @param data Data of the field
 * @param size Size of the field
 * @return 0 if the function succeeded, -1 otherwise
 */
int amp_push(amp_msg_t *amp, amp_type_e type, void *data, size_t size);

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
int amp_encode(amp_msg_t *amp, void **buf, size_t *size);

/**
 * @brief Encode AMP message
 * @param amp AMP message
 * @param buf Buffer to which the AMP message should be encoded
 * @param size return the size of the buffer
 * @return 0 if the function succeeded, -1 otherwise
 */
int amp_decode(amp_msg_t *amp, void **buf, size_t *size);

/**
 * @brief Get number of fields in AMP message
 * @param amp AMP message
 * @return Number of fields if the function succeeded, -1 otherwise
 */
int amp_get_count(amp_msg_t *amp);

/**
 * @brief Get first field in AMP message
 * @param amp AMP message
 * @return First field of the message if the function succeeded, NULL otherwise
 */
amp_field_t *amp_get_first(amp_msg_t *amp);

/**
 * @brief Get next field in AMP message
 * @param amp AMP message
 * @return Next field of the message if the function succeeded, NULL otherwise
 */
amp_field_t *amp_get_next(amp_msg_t *amp);

/**
 * @brief Release AMP message
 * @param amp AMP message
 */
void amp_release(amp_msg_t *amp);
