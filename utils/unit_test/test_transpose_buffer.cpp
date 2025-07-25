#include <gtest/gtest.h>

#include <cstdint>
#include <transpose_buffer.h>

TEST(TransposeBuffer, Base) {
    /// Rows = 1, Columns = 1
    {
        constexpr uint8_t input_buffer[] = {1};
        uint8_t output_buffer[] = {0u};

        transpose_buffer(
            input_buffer,
            output_buffer,
            1,
            1);
        EXPECT_EQ(output_buffer[0], input_buffer[0]);
    }

    /// Either rows or columns is 1
    {
        constexpr uint8_t input_buffer[] = {1, 2, 3, 4, 5};
        constexpr uint32_t input_buffer_length = sizeof(input_buffer) / sizeof(uint8_t);

        /// rows = 1
        {
            constexpr uint32_t num_columns = input_buffer_length;
            constexpr uint32_t num_rows = 1;
            uint8_t output_buffer[input_buffer_length];

            transpose_buffer(
                input_buffer,
                output_buffer,
                num_rows,
                num_columns);

            const int memcmp_result = memcmp(
                output_buffer,
                input_buffer,
                sizeof(input_buffer));
            EXPECT_EQ(memcmp_result, 0);
        }

        /// columns = 1
        {
            constexpr uint32_t num_columns = 1;
            constexpr uint32_t num_rows = input_buffer_length;
            uint8_t output_buffer[input_buffer_length];

            transpose_buffer(
                input_buffer,
                output_buffer,
                num_rows,
                num_columns);
            const int memcmp_result = memcmp(
                output_buffer,
                input_buffer,
                sizeof(input_buffer));
            EXPECT_EQ(memcmp_result, 0);
        }
    }
}

TEST(TransposeBuffer, Typical) {
    {
        constexpr uint8_t original_buffer[] = {
            1, 2, 3,
            4, 5, 6
        };
        constexpr uint32_t original_buffer_num_rows = 2;
        constexpr uint32_t original_buffer_num_coluimns = 3;
        assert(original_buffer_num_rows * original_buffer_num_coluimns == sizeof(original_buffer));

        constexpr uint8_t transposed_buffer[] = {
            1, 4,
            2, 5,
            3, 6
        };
        constexpr uint32_t transposed_buffer_num_rows = 3;
        constexpr uint32_t transposed_buffer_num_coluimns = 2;
        assert(transposed_buffer_num_rows * transposed_buffer_num_coluimns == sizeof(transposed_buffer));
        assert(sizeof(transposed_buffer) == sizeof(original_buffer));

        /// Perform transpose
        {
            uint8_t output_buffer[
                original_buffer_num_rows * original_buffer_num_coluimns];

            transpose_buffer(
                original_buffer,
                output_buffer,
                original_buffer_num_rows,
                original_buffer_num_coluimns);
            const int memcmp_result = memcmp(
                output_buffer,
                transposed_buffer,
                sizeof(transposed_buffer));
            EXPECT_EQ(memcmp_result, 0);
        }

        /// Perform transpose again
        {
            uint8_t output_buffer[
                transposed_buffer_num_rows * transposed_buffer_num_coluimns];

            transpose_buffer(
                transposed_buffer,
                output_buffer,
                transposed_buffer_num_rows,
        transposed_buffer_num_coluimns);
            const int memcmp_result = memcmp(
                output_buffer,
                original_buffer,
                sizeof(original_buffer));
            EXPECT_EQ(memcmp_result, 0);
        }
    }
}
