import unittest

import scipy.signal
from python_interface.scaler_c import scaler_c
import sys
import glob
import numpy as np

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
scaler_c_lib = scaler_c(library_path=LIBRARY_PATH)


class Scaler_PythonTestCase(unittest.TestCase):
    def test_scaler_float(self):
        ERROR_TOLERANCE_DELTA = 1e-7
        ROUNDING_ERROR_TOLERANCE = 1.1
        UINT8_MAX = np.iinfo(np.uint8).max
        UINT8_MIN = np.iinfo(np.uint8).min

        # Prepare input array, top_value and bottom_value

        my_input_float_list = [0.0, -25.0, 1e-7, 5 * 1e-9, 2000, 100.0, -9999.999992]
        my_input_float_np_array = np.array(my_input_float_list, dtype=np.float32)

        np_min_my_input = np.min(my_input_float_np_array)
        np_max_my_input = np.max(my_input_float_np_array)

        min_values = [np_min_my_input, np_min_my_input + 200, np_min_my_input - 200, -80.0]
        max_values = [np_max_my_input, np_max_my_input + 200, np_max_my_input - 200, 0.0]
        assert (np.min(min_values) < np.max(max_values))

        num_elements = len(my_input_float_list)

        # Iterate through different min_values and max_values
        for min_value in min_values:
            for max_value in max_values:
                value_range = max_value - min_value
                expected_results_float = (my_input_float_np_array - min_value) / value_range
                expected_results_float = np.clip(
                    expected_results_float,
                    a_min=0.0,
                    a_max=1.0)
                expected_results_uint8 = (expected_results_float * UINT8_MAX).astype(np.uint8)
                assert (len(expected_results_float) == len(expected_results_uint8))

                output_buffer_float_from_list = scaler_c_lib.scale_float_buffer(
                    input_buffer_float=my_input_float_list,
                    top_value_float=max_value,
                    bottom_value_float=min_value)
                output_buffer_float_from_np_array = scaler_c_lib.scale_float_buffer(
                    input_buffer_float=my_input_float_np_array,
                    top_value_float=max_value,
                    bottom_value_float=min_value)

                output_buffer_quantised_from_list = scaler_c_lib.scale_float_buffer_quantised(
                    input_buffer_float=my_input_float_list,
                    top_value_float=max_value,
                    bottom_value_float=min_value)
                output_buffer_quantised_from_np_array = scaler_c_lib.scale_float_buffer_quantised(
                    input_buffer_float=my_input_float_np_array,
                    top_value_float=max_value,
                    bottom_value_float=min_value)

                # Check lengths
                self.assertEqual(
                    len(output_buffer_float_from_list),
                    num_elements
                )
                self.assertEqual(
                    len(output_buffer_float_from_list),
                    len(output_buffer_float_from_np_array)
                )
                self.assertEqual(
                    len(output_buffer_quantised_from_list),
                    len(output_buffer_quantised_from_np_array)
                )
                self.assertEqual(
                    len(output_buffer_float_from_list),
                    len(output_buffer_quantised_from_list)
                )

                # Check bounds for float
                self.assertLessEqual(np.max(output_buffer_float_from_list), 1.0)
                self.assertLessEqual(np.max(output_buffer_float_from_np_array), 1.0)
                self.assertGreaterEqual(np.min(output_buffer_float_from_list), 0.0)
                self.assertGreaterEqual(np.min(output_buffer_float_from_np_array), 0.0)

                # Check bounds for uint8
                self.assertLessEqual(np.max(output_buffer_quantised_from_list), UINT8_MAX)
                self.assertLessEqual(np.max(output_buffer_quantised_from_np_array), UINT8_MAX)
                self.assertGreaterEqual(np.min(output_buffer_quantised_from_list), UINT8_MIN)
                self.assertGreaterEqual(np.min(output_buffer_quantised_from_np_array), UINT8_MIN)

                # Check results
                for element_iterator in (range(0, num_elements)):
                    self.assertAlmostEqual(
                        first=output_buffer_float_from_list[element_iterator],
                        second=expected_results_float[element_iterator],
                        delta=ERROR_TOLERANCE_DELTA,
                    )
                    self.assertAlmostEqual(
                        first=output_buffer_float_from_np_array[element_iterator],
                        second=expected_results_float[element_iterator],
                        delta=ERROR_TOLERANCE_DELTA,
                    )
                    self.assertAlmostEqual(
                        first=float(output_buffer_quantised_from_list[element_iterator]),
                        second=float(expected_results_uint8[element_iterator]),
                        delta=ROUNDING_ERROR_TOLERANCE,
                    )
                    self.assertAlmostEqual(
                        first=float(output_buffer_quantised_from_np_array[element_iterator]),
                        second=float(expected_results_uint8[element_iterator]),
                        delta=ROUNDING_ERROR_TOLERANCE,
                    )
                    self.assertAlmostEqual(
                        first=output_buffer_float_from_np_array[element_iterator] * UINT8_MAX,
                        second=float(output_buffer_quantised_from_np_array[element_iterator].astype(np.float32)),
                        delta=ROUNDING_ERROR_TOLERANCE,
                    )


if __name__ == '__main__':
    unittest.main()
