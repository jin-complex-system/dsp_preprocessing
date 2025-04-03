import unittest
from python_interface.statistics_c import statistics_c, STRUCT_STATISTICS, STATISTICS_ELEMENT_LENGTH
import sys
import glob
import numpy as np
import ctypes

CURRENT_BINARY_LOCATION = sys.argv[2]
LIBRARY_PATH = glob.glob(CURRENT_BINARY_LOCATION + "/*_shared.dll")[0]
stats_lib = statistics_c(library_path=LIBRARY_PATH)

FLOAT_ERROR_NUM_PLACES = 5


class PythonTestUtils(unittest.TestCase):
    def test_statistics_structure(self):
        my_statistics_structure = STRUCT_STATISTICS()
        my_statistics_structure_size = ctypes.sizeof(my_statistics_structure)
        self.assertNotEqual(0, my_statistics_structure_size)

        self.assertEqual(
            STATISTICS_ELEMENT_LENGTH,
            my_statistics_structure_size / ctypes.sizeof(ctypes.c_float))

        # Test default values
        self.assertEqual(my_statistics_structure.max, float(0.0))
        self.assertEqual(my_statistics_structure.min, float(0.0))
        self.assertEqual(my_statistics_structure.mean, float(0.0))
        self.assertEqual(my_statistics_structure.median, float(0.0))
        self.assertEqual(my_statistics_structure.std_dev, float(0.0))

    def test_statistics_with_random_input(self):
        rng = np.random.default_rng()
        random_input_num_frames = 20
        random_input_bin_length = 10

        random_input_2D = 10 * rng.random(
            size=(random_input_num_frames, random_input_bin_length),
            dtype=np.float32)
        assert (random_input_2D.shape[0] == random_input_num_frames)
        assert (random_input_2D.shape[1] == random_input_bin_length)

        random_result_list_2D = stats_lib.compute_statistics_across_frames(
            input_array_float=random_input_2D,
            num_frames_uint32=random_input_num_frames,
            bin_length_uint32=random_input_bin_length,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(random_result_list_2D), random_input_bin_length * STATISTICS_ELEMENT_LENGTH)

        # Compare results to test_compute_statistics_within_frame()
        for bin_iterator in range(0, random_input_bin_length):

            # Make sure to do a deep copy
            one_frame = np.copy(random_input_2D[:, bin_iterator])
            assert (len(one_frame) == random_input_num_frames)

            current_result_bin_iterator = int(bin_iterator * STATISTICS_ELEMENT_LENGTH)
            assert(current_result_bin_iterator < STATISTICS_ELEMENT_LENGTH * random_input_bin_length)

            random_frame_result_list = stats_lib.compute_statistics_within_frame(
                input_array_float=one_frame,
                bin_length_uint32=random_input_num_frames,
                frame_difference_reciprocal_float=float(1.0),
            )
            self.assertEqual(len(random_frame_result_list), STATISTICS_ELEMENT_LENGTH)

            assert_fail_msg = ("Failed at bin {} for "
                               "input frame\r\n{}\r\n"
                               "within_frame: {} vs across_frame at bin {}: {}\r\n").format(
                bin_iterator,
                one_frame,
                random_frame_result_list,
                bin_iterator,
                random_result_list_2D[current_result_bin_iterator + 0:current_result_bin_iterator + 4])

            self.assertEqual(
                random_frame_result_list[0],
                random_result_list_2D[current_result_bin_iterator + 0],
                msg="{}".format(assert_fail_msg))
            self.assertEqual(
                random_frame_result_list[1],
                random_result_list_2D[current_result_bin_iterator + 1],
                msg="{}".format(assert_fail_msg))
            self.assertEqual(
                random_frame_result_list[2],
                random_result_list_2D[current_result_bin_iterator + 2],
                msg="{}".format(assert_fail_msg))
            self.assertEqual(
                random_frame_result_list[3],
                random_result_list_2D[current_result_bin_iterator + 3],
                msg="{}".format(assert_fail_msg))
            self.assertEqual(
                random_frame_result_list[4],
                random_result_list_2D[current_result_bin_iterator + 4],
                msg="{}".format(assert_fail_msg))

    def test_statistics_across_frames(self):
        # All zeros
        zero_input_frame_num_frames = 20
        zero_input_frame_bin_length = 10

        zeros_input_array2D = np.zeros(
            shape=(zero_input_frame_num_frames, zero_input_frame_bin_length), dtype=np.float32)
        zeros_input_array1D = np.reshape(zeros_input_array2D, shape=-1, copy=True)
        assert (
                zeros_input_array2D.shape[0] == zero_input_frame_num_frames and
                zeros_input_array2D.shape[1] == zero_input_frame_bin_length)
        assert (zeros_input_array1D.shape[0] == zero_input_frame_num_frames * zero_input_frame_bin_length)

        zeros_result_list_1D = stats_lib.compute_statistics_across_frames(
            input_array_float=zeros_input_array1D,
            num_frames_uint32=zero_input_frame_num_frames,
            bin_length_uint32=zero_input_frame_bin_length,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(zeros_result_list_1D), zero_input_frame_bin_length * STATISTICS_ELEMENT_LENGTH)

        zeros_result_list_2D = stats_lib.compute_statistics_across_frames(
            input_array_float=zeros_input_array2D,
            num_frames_uint32=zero_input_frame_num_frames,
            bin_length_uint32=zero_input_frame_bin_length,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(zeros_result_list_2D), zero_input_frame_bin_length * STATISTICS_ELEMENT_LENGTH)

        for iterator in range(0, int(zero_input_frame_bin_length * STATISTICS_ELEMENT_LENGTH)):
            self.assertEqual(
                zeros_result_list_1D[iterator],
                float(0.0),
                msg="{} {} {}".format(iterator, zeros_result_list_1D[iterator], 0.0))
            self.assertEqual(
                zeros_result_list_2D[iterator],
                float(0.0),
                msg="{} {} {}".format(iterator, zeros_result_list_2D[iterator], 0.0))

        # Basic example 1
        basic_input_1 = np.array([0.0, 0.0, 1.0, 0.0, 0.0], dtype=np.float32)
        assert (basic_input_1.shape[0] == len(basic_input_1))

        result_list_1 = stats_lib.compute_statistics_across_frames(
            input_array_float=basic_input_1,
            num_frames_uint32=len(basic_input_1),
            bin_length_uint32=1,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(result_list_1), STATISTICS_ELEMENT_LENGTH)
        self.assertEqual(result_list_1[0], np.max(basic_input_1))
        self.assertEqual(result_list_1[1], np.min(basic_input_1))
        self.assertEqual(result_list_1[2], np.mean(basic_input_1))
        self.assertEqual(result_list_1[3], np.median(basic_input_1))
        self.assertEqual(result_list_1[4], np.std(basic_input_1))

        # Basic example 2
        basic_input_2 = np.array([0.0, 0.0, 0.0, 0.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0], dtype=np.float32)
        assert (basic_input_2.shape[0] == len(basic_input_2))

        result_list_2 = stats_lib.compute_statistics_across_frames(
            input_array_float=basic_input_2,
            num_frames_uint32=len(basic_input_2),
            bin_length_uint32=1,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(result_list_2), STATISTICS_ELEMENT_LENGTH)
        self.assertEqual(result_list_2[0], np.max(basic_input_2))
        self.assertEqual(result_list_2[1], np.min(basic_input_2))
        self.assertEqual(result_list_2[2], np.mean(basic_input_2))
        self.assertEqual(result_list_2[3], np.median(basic_input_2))
        self.assertEqual(result_list_2[4], np.std(basic_input_2))

        # Typical example
        typical_input = np.array([
            [1, 4, 0],      # Frame 1
            [2, 5, 9],      # Frame 2
            [3, -2, 0],     # Frame 3
            [8, 0, 0],      # Frame 4
        ], dtype=np.float32)
        basic_input_num_frames = typical_input.shape[0]
        basic_input_bin_length = typical_input.shape[1]

        bin_1 = typical_input[:, 0]
        assert(bin_1.all() == np.array([1, 2, 3, 8]).all())
        bin_2 = typical_input[:, 1]
        assert(bin_2.all() == np.array([4, 5, -2, 0]).all())
        bin_3 = typical_input[:, 2]
        assert(bin_3.all() == np.array([0, 9, 0, 0]).all())

        expected_result_basic_input = np.array([
            np.max(bin_1), np.min(bin_1), np.mean(bin_1), np.median(bin_1), np.std(bin_1),          # bin 1
            np.max(bin_2), np.min(bin_2), np.mean(bin_2), np.median(bin_2), np.std(bin_2),          # bin 2
            np.max(bin_3), np.min(bin_3), np.mean(bin_3), np.median(bin_3), np.std(bin_3),          # bin 3
        ], dtype=np.float32)
        assert(len(expected_result_basic_input) == basic_input_bin_length * STATISTICS_ELEMENT_LENGTH)

        typical_input_result_list_2D = stats_lib.compute_statistics_across_frames(
            input_array_float=typical_input,
            num_frames_uint32=basic_input_num_frames,
            bin_length_uint32=basic_input_bin_length,
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(typical_input_result_list_2D), basic_input_bin_length * STATISTICS_ELEMENT_LENGTH)
        self.assertEqual(len(typical_input_result_list_2D), len(expected_result_basic_input))

        for iterator in range(0, basic_input_bin_length * STATISTICS_ELEMENT_LENGTH):
            assert_error_msg = "Mismatch at iterator {}\r\n{}\r\nvs\r\n{}\r\n".format(
                iterator,
                typical_input_result_list_2D,
                expected_result_basic_input)

            self.assertEqual(
                typical_input_result_list_2D[iterator],
                expected_result_basic_input[iterator],
                msg="{}".format(assert_error_msg)
            )

    def test_compute_statistics_within_frame(self):
        # All zeros
        zero_input_list_length = 2
        zeros_input_list = np.zeros(shape=zero_input_list_length, dtype=np.float32)
        assert (zeros_input_list.shape[0] == zero_input_list_length)

        zeros_result_list = stats_lib.compute_statistics_within_frame(
            input_array_float=zeros_input_list,
            bin_length_uint32=len(zeros_input_list),
            frame_difference_reciprocal_float=float(1.0),
        )

        self.assertEqual(len(zeros_result_list), STATISTICS_ELEMENT_LENGTH)
        for stats_zero in zeros_result_list:
            self.assertEqual(stats_zero, 0.0)

        # Basic example
        input_array = np.array([0.0, 0.0, 1.0, 0.0, 0.0], dtype=np.float32)
        assert (input_array.shape[0] == len(input_array))

        result_list = stats_lib.compute_statistics_within_frame(
            input_array_float=input_array,
            bin_length_uint32=len(input_array),
            frame_difference_reciprocal_float=float(1.0),
        )
        self.assertEqual(len(result_list), STATISTICS_ELEMENT_LENGTH)
        self.assertEqual(result_list[0], np.max(input_array))
        self.assertEqual(result_list[1], np.min(input_array))
        self.assertEqual(result_list[2], np.mean(input_array))
        self.assertEqual(result_list[3], np.median(input_array))
        self.assertEqual(result_list[4], np.std(input_array))


if __name__ == '__main__':
    unittest.main()
