#ifndef TRANSPOSE_BUFFER_H
#define TRANSPOSE_BUFFER_H

#include <stdint.h>

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/**
 * Transpose buffer
 * @param pSource
 * @param pDest
 * @param num_rows
 * @param num_columns
 */
void
transpose_buffer(
    const uint8_t* pSource,
    uint8_t* pDest,
    const uint32_t num_rows,
    const uint32_t num_columns);

/* Provide C++ Compatibility */
#ifdef __cplusplus
};
#endif //__cplusplus

#endif //TRANSPOSE_BUFFER_H
