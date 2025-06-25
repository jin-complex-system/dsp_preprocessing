import os
import sys
import time


def save_plots(
        mel_spectrogram,
        hop_length,
        n_fft,
        win_length,
        target_directory,
        postfix_str,
):
    """
    Save plots of power spectrum and mel spectrogram
    :param mel_spectrogram:
    :param hop_length:
    :param n_fft:
    :param win_length:
    :param target_directory:
    :param postfix_str: Identifier to distinguish figures
    :return:
    """
    # Import necessary libraries
    import matplotlib.pyplot as plt
    import librosa
    import os

    # Create directory if it does not exist before
    os.makedirs(target_directory, exist_ok=True)

    # Save mel spectrogram
    plt.figure()
    librosa.display.specshow(
        mel_spectrogram,
        hop_length=hop_length,
        win_length=win_length,
        n_fft=n_fft,
        cmap="magma")
    plt.axis("off")
    plt.savefig(
        os.path.join(
            target_directory,
            "mel_spec_{}.png".format(postfix_str)),
        bbox_inches='tight',
        pad_inches=0)
    plt.close()


def compute_and_plot(
        library_binary_location,
        audio_dsp_python_interface_filepath,
        audio_filepath,
        audio_type,
        n_fft,
        hop_length,
        n_mel,
        max_frequencies,
        top_decibel,
        output_directory
):
    """ Run different examples of approaches to process audio

    :param library_binary_location:
    :param audio_dsp_python_interface_filepath:
    :param audio_filepath:
    :param audio_type:
    :param n_fft:
    :param hop_length:
    :param n_mel: number of mel bins in a mel spectrogram
    :param max_frequencies: list of max frequencies
    :param top_decibel: top decibel to clip
    :param output_directory:
    :return:
    """
    # Import necessary libraries
    import numpy as np
    import librosa

    # Load the wave file as float
    # Note that we are re-sampling to 44.1 kHz and forcing used of mono
    samples_float, sample_rate = librosa.load(
        audio_filepath,
        sr=44100,
        mono=True,
        dtype=np.float32,
    )

    # Forcefully load samples back as audio_type
    scaling_factor_float_to_audio_type = np.iinfo(audio_type).max
    scaling_factor = float(1.0 / scaling_factor_float_to_audio_type)
    samples = np.array(
        [int(s * scaling_factor_float_to_audio_type) for s in samples_float],
        dtype=audio_type,
    )

    # Create the output directory for the current audio file
    audio_filename_with_ext = os.path.basename(audio_filepath)
    audio_filename = audio_filename_with_ext.split('.')[0]

    for max_frequency in max_frequencies:
        if max_frequency is None or max_frequency == 0:
            max_frequency = int(sample_rate / 2)
        max_frequency_directory = "fmax_{}".format(max_frequency)

        target_directory = os.path.join(
            output_directory,
            max_frequency_directory,
            audio_filename
        )
        os.makedirs(target_directory, exist_ok=True)

        print("\r\n")
        print("Processing {} into {}, sample_rate - {}, fmax - {}".format(
            audio_filepath,
            target_directory,
            sample_rate,
            max_frequency,
        ))

        # Using audio_dsp's python interface (precompute and no precompute)
        # compute_and_plot_audio_dsp_c(
        #     library_binary_location=library_binary_location,
        #     audio_dsp_python_interface_filepath=audio_dsp_python_interface_filepath,
        #     samples=samples,
        #     sample_rate=sample_rate,
        #     scaling_factor=scaling_factor,
        #     n_fft=n_fft,
        #     hop_length=hop_length,
        #     n_mel=n_mel,
        #     max_frequency=max_frequency,
        #     top_decibel=top_decibel,
        #     use_precompute=True,
        #     target_directory=target_directory,
        # )
        compute_and_plot_audio_dsp_c(
            library_binary_location=library_binary_location,
            audio_dsp_python_interface_filepath=audio_dsp_python_interface_filepath,
            samples=samples,
            sample_rate=sample_rate,
            scaling_factor=scaling_factor,
            n_fft=n_fft,
            hop_length=hop_length,
            n_mel=n_mel,
            max_frequency=max_frequency,
            top_decibel=top_decibel,
            use_precompute=False,
            target_directory=target_directory,
        )
        compute_and_plot_librosa(
            samples=samples,
            sample_rate=sample_rate,
            scaling_factor=scaling_factor,
            n_fft=n_fft,
            hop_length=hop_length,
            n_mel=n_mel,
            max_frequency=max_frequency,
            top_decibel=top_decibel,
            target_directory=target_directory,
        )


def compute_and_plot_librosa(
        samples,
        sample_rate,
        scaling_factor,
        n_fft,
        hop_length,
        n_mel,
        max_frequency,
        top_decibel,
        target_directory,
):
    """ Compute mel spectrogram using librosa library and plot the figures

    :param samples:
    :param sample_rate: frequency of audio file
    :param scaling_factor: scaling factor; usually the range of the audio representation
    :param n_fft:
    :param hop_length:
    :param n_mel: number of mel bins in a mel spectrogram
    :param max_frequency: max frequency of mel spectrogram; usually represented as sample_rate / 2
    :param top_decibel: top decibel to clip
    :param target_directory:
    :return:
    """

    # Necessary libraries
    import librosa
    import numpy as np

    # Check parameters
    assert (isinstance(top_decibel, float))

    # Librosa version
    power_spectrum_librosa_start_time = time.time()
    samples_float = samples.astype(np.float32) / scaling_factor
    assert (samples_float.dtype == np.float32)
    stft_librosa = librosa.stft(
        y=samples_float,
        n_fft=n_fft,
        hop_length=hop_length,
        win_length=n_fft,
        center=False,
        # dtype=np.float32, # Gives a warning about casting complex values to real
    )
    power_spectrum_librosa = np.abs(stft_librosa) ** 2
    power_spectrum_librosa_end_time = time.time()
    power_spectrum_librosa_total_time = power_spectrum_librosa_end_time - power_spectrum_librosa_start_time

    mel_spectrogram_librosa_start_time = time.time()
    mel_spectrogram_librosa = librosa.feature.melspectrogram(
        S=power_spectrum_librosa,
        n_mels=n_mel,
        dtype=np.float32,
        fmax=max_frequency,
    )
    mel_spectrogram_librosa = librosa.power_to_db(
        S=mel_spectrogram_librosa,
        ref=np.max,
        amin=1e-30,
        top_db=top_decibel,
    )
    mel_spectrogram_librosa_end_time = time.time()
    mel_spectrogram_librosa_total_time = mel_spectrogram_librosa_end_time - mel_spectrogram_librosa_start_time

    # Save plots from librosa
    save_plots(
        mel_spectrogram=mel_spectrogram_librosa,
        hop_length=hop_length,
        n_fft=n_fft,
        win_length=n_fft,
        target_directory=os.path.join(
            target_directory, "librosa"),
        postfix_str="librosa",
    )

    print("Librosa - power spectrum: {} s, mel spectrogram: {} s".format(
        power_spectrum_librosa_total_time,
        mel_spectrogram_librosa_total_time,
    ))

    max_value = np.max(mel_spectrogram_librosa)
    min_value = np.min(mel_spectrogram_librosa)
    median_value = np.median(mel_spectrogram_librosa)
    mean_value = np.mean(mel_spectrogram_librosa)
    std_value = np.std(mel_spectrogram_librosa)
    print("Statistics for librosa - max: {}, min: {}, median: {}, mean: {}, std: {}".format(
        max_value, min_value, median_value, mean_value, std_value
    ))


def compute_and_plot_audio_dsp_c(
        library_binary_location,
        audio_dsp_python_interface_filepath,
        samples,
        sample_rate,
        scaling_factor,
        n_fft,
        hop_length,
        n_mel,
        max_frequency,
        top_decibel,
        use_precompute,
        target_directory,
):
    """
    Compute mel spectrogram using audio_dsp library and plot the figures

    :param library_binary_location: location of C library
    :param audio_dsp_python_interface_filepath:
    :param samples:
    :param sample_rate: frequency of audio file
    :param scaling_factor: scaling factor; usually the range of the audio representation
    :param n_fft:
    :param hop_length:
    :param n_mel: number of mel bins in a mel spectrogram
    :param max_frequency: max frequency of mel spectrogram; usually represented as sample_rate / 2
    :param top_decibel: top decibel to clip
    :param use_precompute: use precomputed values if available
    :param target_directory:
    :return:
    """
    # Necessary libraries
    import importlib.util
    import math
    import numpy as np

    # Check parameters
    assert (isinstance(top_decibel, float))
    assert (use_precompute is False)  # TODO: At the moment, precomputed values is buggy

    # Import audio_dsp_c module, based on python interface file location
    audio_dsp_spec = importlib.util.spec_from_file_location(
        "audio_dsp",
        audio_dsp_python_interface_filepath)
    audio_dsp = importlib.util.module_from_spec(audio_dsp_spec)
    sys.modules["audio_dsp"] = audio_dsp
    audio_dsp_spec.loader.exec_module(audio_dsp)

    # Import the shared library
    audio_dsp_c_lib = audio_dsp.audio_dsp_c(
        library_path=library_binary_location)

    # Assume hann window has been precomputed
    if use_precompute:
        hann_window_scaled = audio_dsp_c_lib.hann_window_compute(
            length_uint32=n_fft,
            scaling_factor_float=scaling_factor,
        )
    else:
        hann_window_scaled = None

    # Compute power spectrum (real FFT log as decibels)
    #
    # We manually compute each frame because in C
    # we may take advantage of reusing buffers
    num_samples = len(samples)
    num_frames = math.floor(num_samples / hop_length - n_fft / hop_length) + 1

    power_spectrum = np.zeros((math.floor(n_fft / 2) + 1, num_frames), dtype=np.float32)
    power_spectrum_start_time = time.time()
    for frame_iterator in range(num_frames):
        begin_iterator = frame_iterator * hop_length
        end_iterator = begin_iterator + n_fft

        # Note that we do not pad sample_frame with zeros
        power_spectrum[:, frame_iterator] = audio_dsp_c_lib.compute_power_spectrum(
            input_samples_array=samples[begin_iterator:end_iterator],
            n_fft_uint16=n_fft,
            window_list=hann_window_scaled,
        )
    power_spectrum_end_time = time.time()
    power_spectrum_total_time = power_spectrum_end_time - power_spectrum_start_time

    # Compute mel spectrogram
    #
    # We manually compute each frame because in C
    # we may take advantage of reusing buffers
    mel_spectrogram = np.zeros(shape=(n_mel, num_frames), dtype=np.float32)
    reference_float = -127.0
    mel_spectrogram_start_time = time.time()
    for frame_iterator in range(0, num_frames):

        if use_precompute:
            mel_spectrogram[:, frame_iterator], max_mel = (
                audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram(
                    power_spectrum_array=power_spectrum[:, frame_iterator],
                    n_mel_uint16=n_mel,
                    n_fft_uint16=n_fft,
                    sample_rate_uint16=sample_rate,
                    max_frequency_uint16=max_frequency,
                ))
        else:
            mel_spectrogram[:, frame_iterator], max_mel = (
                audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram_raw(
                    power_spectrum_array=power_spectrum[:, frame_iterator],
                    n_mel_uint16=n_mel,
                    n_fft_uint16=n_fft,
                    sample_rate_uint16=sample_rate,
                    max_frequency_uint16=max_frequency,
                ))

        if max_mel > reference_float:
            reference_float = max_mel

    mel_spectrogram = np.reshape(mel_spectrogram, shape=-1)
    mel_spectrogram_decibels = audio_dsp_c_lib.convert_power_to_decibel(
        spectrogram_array_float32=mel_spectrogram,
        spectrogram_array_length_uint16=len(mel_spectrogram),
        reference_power_float32=reference_float,
        top_decibel_float32=top_decibel,
    )

    mel_spectrogram_end_time = time.time()
    mel_spectrogram_total_time = mel_spectrogram_end_time - mel_spectrogram_start_time

    if use_precompute:
        postfix_str = "audio_dsp_c_precompute"
        indicator_str = "Precompute"
    else:
        postfix_str = "audio_dsp_c_no_precompute"
        indicator_str = "No Precompute"

    mel_spectrogram_decibels = np.reshape(mel_spectrogram_decibels, shape=(n_mel, num_frames))
    save_plots(
        mel_spectrogram=mel_spectrogram_decibels,
        hop_length=hop_length,
        n_fft=n_fft,
        win_length=n_fft,
        target_directory=os.path.join(
            target_directory, postfix_str),
        postfix_str=postfix_str,
    )
    print("Audio DSP ({}) - power spectrum: {} s, mel spectrogram: {} s".format(
        indicator_str,
        power_spectrum_total_time,
        mel_spectrogram_total_time,
    ))

    max_value = np.max(mel_spectrogram_decibels)
    min_value = np.min(mel_spectrogram_decibels)
    median_value = np.median(mel_spectrogram_decibels)
    mean_value = np.mean(mel_spectrogram_decibels)
    std_value = np.std(mel_spectrogram_decibels)

    print("Statistics for ({}) - max: {}, min: {}, median: {}, mean: {}, std: {}".format(
        indicator_str, max_value, min_value, median_value, mean_value, std_value
    ))


def _main():
    # Import libraries
    import numpy as np

    audio_filepaths = [
        os.path.join("examples", "siren.wav"),
        os.path.join("examples", "street_traffic.wav"),
        os.path.join("examples", "human_voice.wav"),
    ]
    output_directory = os.path.join(
        "examples",
        "_output")

    # Gather system arguments
    library_binary_location = sys.argv[1]
    audio_dsp_python_interface_filepath = sys.argv[2]

    # Some parameters are powers of 2 to take advantage of hardware
    audio_type = np.int16
    n_fft = 2048
    n_mel = 64
    hop_length = int(n_fft / 4)
    top_decibel = 80.0
    max_frequencies = [
        0,  # Use sample_rate / 2
        8000
    ]

    print("audio_type - {}, n_fft - {}, hop_length - {}, n_mel - {}".format(
        audio_type,
        n_fft,
        hop_length,
        n_mel,
    ))

    for audio_filepath in audio_filepaths:
        compute_and_plot(
            library_binary_location=library_binary_location,
            audio_dsp_python_interface_filepath=audio_dsp_python_interface_filepath,
            audio_filepath=audio_filepath,
            audio_type=audio_type,
            n_fft=n_fft,
            hop_length=hop_length,
            n_mel=n_mel,
            max_frequencies=max_frequencies,
            top_decibel=top_decibel,
            output_directory=output_directory,
        )
    print("Done")


if __name__ == '__main__':
    _main()

