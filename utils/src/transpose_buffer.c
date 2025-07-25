#include "transpose_buffer.h"

#include <stddef.h>
#include <assert.h>
#include <string.h>

void
transpose_buffer(
    const uint8_t* pSource,
    uint8_t* pDest,
    const uint32_t num_rows,
    const uint32_t num_columns) {
    /// Check parameters
    {
        assert(num_rows > 0 && num_columns > 0);
        assert(pSource != NULL);
        assert(pDest != NULL);
    }

    /// Base case
    if (num_rows == 1 && num_columns == 1) {
        pDest[0] = pSource[0];
    }
    /// Just do a memcpy
    else if (num_rows == 1 || num_columns == 1) {
        memcpy(
            pDest,
            pSource,
            num_columns * num_rows * sizeof(uint8_t));
    }
    else {
        /// https://stackoverflow.com/questions/16737298/what-is-the-fastest-way-to-transpose-a-matrix-in-c

        #pragma omp parallel for
        for(uint32_t n = 0; n < num_rows * num_columns; n++) {
            const uint32_t i = n / num_rows;
            const uint32_t j = n % num_rows;
            pDest[n] = pSource[num_columns * j + i];
        }
    }
}
