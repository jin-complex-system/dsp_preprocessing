import unittest
from python_interface.utils_c import utils_c
import sys
import glob
import math
import librosa
import numpy as np

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
print(LIBRARY_PATH)

utils_lib = utils_c(library_path=LIBRARY_PATH)

FLOAT_ERROR_NUM_PLACES = 4

MINIMUM_VALUE_FOR_LOG = -37.929779052734375
MINIMUM_FLOAT_VALUE = 1.175494351e-38

class Utils_PythonTestCase(unittest.TestCase):
    def test_compute_power_from_complex_arrays(self):
        values_list = [
            0.0, 2000.0, 0.001, 1.0, -1.0, 2.0, -2.0, 4.0, 16.0, 0.2, -0.65, 99.0,
            200.0, 150.0, 3.1, -1e-5, 0.0001913713349495083, 5*1e-3,
            3346.006, 3270.84912109375,
        ]
        assert (len(values_list) % 2 == 0 and len(values_list) >= 4)
        absolute_error_tolerance = pow(1, -1 * FLOAT_ERROR_NUM_PLACES)

        # Iteratively compare through the entire values_list
        for real_value_iterator in range(0, len(values_list)):
            for img_value_iterator in range(real_value_iterator, len(values_list)):
                real_value = float(values_list[real_value_iterator])
                img_value = float(values_list[img_value_iterator])

                computed_power_results = utils_lib.compute_power_from_complex_arrays(
                    complex_array_float=[real_value, img_value])
                self.assertEqual(
                    len(computed_power_results),
                    1)
                expected_power = real_value * real_value + img_value * img_value

                relative_error_tolerance = (
                        abs(computed_power_results[0] - expected_power) / absolute_error_tolerance)
                assert_fail_msg = "\r\nReal value: {}, imaginary value: {}".format(
                    real_value, img_value
                )
                self.assertAlmostEqual(
                    expected_power,
                    computed_power_results[0],
                    delta=relative_error_tolerance,
                    msg=assert_fail_msg,
                )

        # Compute through the entire values_list
        computed_power_results = utils_lib.compute_power_from_complex_arrays(
            complex_array_float=values_list)
        self.assertEqual(
            len(computed_power_results),
            len(values_list) / 2)
        for iterator in range(0, len(computed_power_results)):
            real_value = float(values_list[iterator * 2 + 0])
            img_value = float(values_list[iterator * 2 + 1])

            expected_power = real_value * real_value + img_value * img_value

            relative_error_tolerance = (
                    abs(computed_power_results[iterator] - expected_power) / absolute_error_tolerance)
            assert_fail_msg = "\r\nReal value: {}, imaginary value: {}".format(
                real_value, img_value
            )
            self.assertAlmostEqual(
                expected_power,
                computed_power_results[iterator],
                delta=relative_error_tolerance,
                msg=assert_fail_msg,
            )

    def test_compute_magnitude_from_complex_arrays(self):
        values_list = [
            0.0, 2000.0, 0.001, 1.0, -1.0, 2.0, -2.0, 4.0, 16.0, 0.2, -0.65, 99.0,
            200.0, 150.0, 3.1, -1e-5, 0.0001913713349495083, 5*1e-3,
            3346.006, 3270.84912109375,
        ]
        assert (len(values_list) % 2 == 0 and len(values_list) >= 4)
        absolute_error_tolerance = pow(1, -1 * FLOAT_ERROR_NUM_PLACES)

        # Iteratively compare through the entire values_list
        for real_value_iterator in range(0, len(values_list)):
            for img_value_iterator in range(real_value_iterator, len(values_list)):
                real_value = float(values_list[real_value_iterator])
                img_value = float(values_list[img_value_iterator])

                computed_magnitude_results = utils_lib.compute_magnitude_from_complex_arrays(
                    complex_array_float=[real_value, img_value])
                self.assertEqual(
                    len(computed_magnitude_results),
                    1)
                expected_magnitude = math.sqrt(
                    real_value * real_value +
                    img_value * img_value +
                    MINIMUM_FLOAT_VALUE)

                relative_error_tolerance = (
                        abs(computed_magnitude_results[0] - expected_magnitude) / absolute_error_tolerance)
                assert_fail_msg = "\r\nReal value: {}, imaginary value: {}".format(
                    real_value, img_value
                )
                self.assertAlmostEqual(
                    expected_magnitude,
                    computed_magnitude_results[0],
                    delta=relative_error_tolerance,
                    msg=assert_fail_msg,
                )

        # Compute through the entire values_list
        computed_magnitude_results = utils_lib.compute_magnitude_from_complex_arrays(
            complex_array_float=values_list)
        self.assertEqual(
            len(computed_magnitude_results),
            len(values_list) / 2)
        for iterator in range(0, len(computed_magnitude_results)):
            real_value = float(values_list[iterator * 2 + 0])
            img_value = float(values_list[iterator * 2 + 1])

            expected_magnitude = math.sqrt(
                real_value * real_value +
                img_value * img_value +
                MINIMUM_FLOAT_VALUE)
            relative_error_tolerance = (abs(computed_magnitude_results[iterator] - expected_magnitude)
                                        / absolute_error_tolerance)

            assert_fail_msg = "\r\nReal value: {}, imaginary value: {}".format(
                real_value, img_value
            )

            self.assertAlmostEqual(
                expected_magnitude,
                computed_magnitude_results[iterator],
                delta=relative_error_tolerance,
                msg=assert_fail_msg,
            )

    def test_compute_log_and_decibels_from_complex(self):
        # Base case
        self.assertAlmostEqual(
            utils_lib.compute_decibels_from_complex(0.0, 0.0, 0.0),
            MINIMUM_VALUE_FOR_LOG * 10,
            places=FLOAT_ERROR_NUM_PLACES)

        # Compare log to decibels
        float_array = [1.0, 15.0, -20.0, -100.0, 1000.0, 1200.0, -3000.0, 0.1, 0.2]

        for real_value_iterator in range(0, len(float_array)):
            for img_value_iterator in range(real_value_iterator, len(float_array)):
                real_value = float(float_array[real_value_iterator])
                img_value = float(float_array[img_value_iterator])

                # Computed results
                computed_decibel = utils_lib.compute_decibels_from_complex(real_value, img_value, 0.0)
                computed_log = utils_lib.compute_log_from_complex(real_value, img_value, 0.0)

                # Expected results
                magnitude = math.sqrt(real_value * real_value + img_value * img_value)
                expected_decibel = 20 * math.log10(magnitude)
                expected_log = math.log10(magnitude)
                librosa_decibel = librosa.amplitude_to_db(S=magnitude)

                self.assertAlmostEqual(
                    computed_decibel,
                    expected_decibel,
                    places=FLOAT_ERROR_NUM_PLACES,
                )
                self.assertAlmostEqual(
                    computed_log,
                    expected_log,
                    places=FLOAT_ERROR_NUM_PLACES,
                )
                self.assertAlmostEqual(
                    computed_decibel,
                    librosa_decibel,
                    places=FLOAT_ERROR_NUM_PLACES,
                )

    def test_insertion_sort(self):
        initial_list = np.array([0.0, 1.0, -1.0, -2.0, 0.5, 2.0, -2.0, -0.5, -0.1], dtype=np.float32)
        computed_list = np.empty([len(initial_list)], dtype=np.float32)

        for iterator in range(0, len(initial_list)):
            new_value = float(initial_list[iterator])
            utils_lib.insertion_sort(
                new_value,
                computed_list,
                iterator
            )
        initial_list.sort()
        assert (initial_list is not None)

        for iterator in range(0, len(initial_list)):
            self.assertEqual(
                computed_list[iterator],
                initial_list[iterator],
                msg="{} {} {}".format(iterator, computed_list[iterator], initial_list[iterator])
            )

    def test_log_approximation(self):
        # Handling of 0.0 and negative values
        self.assertEqual(
            utils_lib.log2_approximation(0.0),
            utils_lib.log2_approximation(-1.0))
        self.assertEqual(
            utils_lib.log10_approximation(0.0),
            utils_lib.log10_approximation(-1.0))

        # Handling of 1.0
        self.assertEqual(utils_lib.log10_approximation(1.0), 0.0)
        self.assertEqual(utils_lib.log2_approximation(1.0), 0.0)

        # Handling near minimum value of float
        self.assertAlmostEqual(
            utils_lib.log10_approximation(MINIMUM_FLOAT_VALUE),
            math.log10(MINIMUM_FLOAT_VALUE),
            places=FLOAT_ERROR_NUM_PLACES)
        self.assertAlmostEqual(
            utils_lib.log2_approximation(MINIMUM_FLOAT_VALUE),
            math.log2(MINIMUM_FLOAT_VALUE),
            places=FLOAT_ERROR_NUM_PLACES)

    def test_vector_log_approximation(self):
        LOG_E_ERROR_DELTA = 3.829956 * 1e-3

        # Handling of 0.0 and negative values
        self.assertEqual(
            utils_lib.v_loge_approximation(0.0)[0],
            utils_lib.v_loge_approximation(-1.0)[0])

        # Handling of 1.0
        self.assertEqual(utils_lib.v_loge_approximation(1.0)[0], 0.0)

        # Handling near minimum value of float
        self.assertAlmostEqual(
            utils_lib.v_loge_approximation(MINIMUM_FLOAT_VALUE)[0],
            math.log(MINIMUM_FLOAT_VALUE),
            delta=LOG_E_ERROR_DELTA)

    def test_square_root_approximation(self):
        input_value_list = [
            0.0, 1.0, 2.0, 4.0, 3200.0, 10.0, 2.0, 0.5, 0.25, 1e-5, 1e-6,
            (44.72136 ** 2), (3346.006 ** 2), (3270.84912109375 ** 2),
        ]

        absolute_error_tolerance = pow(1, -1 * FLOAT_ERROR_NUM_PLACES)

        for input_value in input_value_list:
            assert_fail_msg = "\r\nOriginal value: {}".format(input_value)

            expected_value = math.sqrt(input_value)
            computed_value = utils_lib.square_root_approximation(input_value)

            relative_error_tolerance = abs(computed_value - expected_value) / absolute_error_tolerance

            self.assertAlmostEqual(
                computed_value,
                expected_value,
                delta=relative_error_tolerance,
                msg=assert_fail_msg,
            )


if __name__ == '__main__':
    unittest.main()
