# c-amp

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/84a24dea00694153ae2f5d72a56b4356)](https://app.codacy.com/gh/joelguittet/c-amp?utm_source=github.com&utm_medium=referral&utm_content=joelguittet/c-amp&utm_campaign=Badge_Grade_Settings)

Abstract Message Protocol codec and streaming parser in C.

This repository is not a fork of [node-amp](https://github.com/tj/node-amp) ! It has the same behavior but it is a complete library written in C in order to be portable and used in various applications.

## Building

Build `libamp.so` with the following commands:

``` bash
cmake .
make
```

## Examples

Build examples with the following commands:
``` bash
cmake .
make amp_examples
```

Examples can executed with:
``` bash
bin/decoder
```
and:
``` bash
bin/encoder
```

## API

### amp_msg_t *amp_create(void)

Initialize an empty message.

### int amp_push(amp_msg_t *amp, amp_type_e type, void *data, size_t size)

Add a new field `data` of type `type` and size `size` to the AMP message.

### int amp_encode(amp_msg_t *amp, void **buf, size_t *size)

Encode AMP message to `buf`. Size of the output is provided with `size`.

### int amp_decode(amp_msg_t *amp, void **buf, size_t *size)

Decode AMP message from `buf` os size `size`.

### int amp_get_count(amp_msg_t *amp)

Return the number of fields in the AMP message.

### amp_field_t *amp_get_first(amp_msg_t *amp)

Get first field of the AMP message.

### amp_field_t *amp_get_next(amp_msg_t *amp)

Get next field of the AMP message.

### void amp_release(amp_msg_t *amp)

Release internal memory. Must be called to free ressources.

## License

MIT
