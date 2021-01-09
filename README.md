# c-amp

[![CMake Badge](https://github.com/joelguittet/c-amp/workflows/CMake%20+%20SonarCloud%20Analysis/badge.svg)](https://github.com/joelguittet/c-amp/actions)
[![Issues Badge](https://img.shields.io/github/issues/joelguittet/c-amp)](https://github.com/joelguittet/c-amp/issues)
[![License Badge](https://img.shields.io/github/license/joelguittet/c-axon)](https://github.com/joelguittet/c-amp/blob/master/LICENSE)

[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=bugs)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=code_smells)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=duplicated_lines_density)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=ncloc)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)

[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=joelguittet_c-amp&metric=security_rating)](https://sonarcloud.io/dashboard?id=joelguittet_c-amp)

Abstract Message Protocol codec and streaming parser in C.

This repository is not a fork of [node-amp](https://github.com/tj/node-amp) ! It has the same behavior but it is a complete library written in C in order to be portable and used in various applications.

## Building

Build `libamp.so` with the following commands:

``` bash
cmake .
make
```

## Compatibility

This library is compatible with [node-amp](https://github.com/tj/node-amp) release 0.3.1.

## Examples

Build examples with the following commands:
``` bash
cmake -DENABLE_AMP_EXAMPLES=ON .
make
```

Examples can executed with:
``` bash
build/bin/decoder
```
and:
``` bash
build/bin/encoder
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
