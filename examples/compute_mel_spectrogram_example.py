import os
import sys
import time


def save_plots(
        power_spectrum,
        mel_spectrogram,
        sample_rate,
        hop_length,
        n_fft,
        win_length,
        target_directory,
        postfix_str,
):
    """
    Save plots of power spectrum and mel spectrogram
    :param power_spectrum:
    :param mel_spectrogram:
    :param sample_rate:
    :param hop_length:
    :param n_fft:
    :param win_length:
    :param target_directory:
    :param postfix_str: Identifier to distinguish figures
    :return:
    """

    import matplotlib.pyplot as plt
    import librosa
    import os

    os.makedirs(target_directory, exist_ok=True)

    # Save power spectrum
    plt.figure()
    librosa.display.specshow(
        power_spectrum,
        sr=sample_rate,
        hop_length=hop_length,
        win_length=win_length,
        n_fft=n_fft,
        cmap="magma")
    plt.axis("off")
    plt.savefig(
        os.path.join(
            target_directory,
            "power_spectrum_{}.png".format(postfix_str)),
        bbox_inches='tight',
        pad_inches=0)
    plt.close()

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


def compute_and_plot_audio_dsp_c(
        library_binary_location,
        audio_dsp_python_interface_filepath,
        samples,
        sample_rate,
        scaling_factor,
        n_fft,
        hop_length,
        n_mel,
        use_precompute,
):
    """
    Compute mel spectrogram and plot the figures; do not precompute
    :param library_binary_location: location of C library
    :param audio_dsp_python_interface_filepath:
    :param samples:
    :param sample_rate: frequency of audio file
    :param scaling_factor: scaling factor; usually the range of the audio representation
    :param n_fft:
    :param hop_length:
    :param n_mel: number of mel bins in a mel spectrogram
    :param use_precompute: use precomputed values if available
    :return:
    """
    # Necessary libraries
    import importlib.util
    import math
    import numpy as np

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
    mel_spectrogram_start_time = time.time()
    for frame_iterator in range(0, num_frames):

        if use_precompute:
            mel_spectrogram[:, frame_iterator] = (
                audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram(
                    power_spectrum_array=power_spectrum[:, frame_iterator],
                    n_mel_uint16=n_mel,
                    n_fft_uint16=n_fft,
                    sample_rate_uint16=sample_rate,
                ))
        else:
            mel_spectrogram[:, frame_iterator] = (
                audio_dsp_c_lib.compute_power_spectrum_into_mel_spectrogram_raw(
                    power_spectrum_array=power_spectrum[:, frame_iterator],
                    n_mel_uint16=n_mel,
                    n_fft_uint16=n_fft,
                    sample_rate_uint16=sample_rate,
                ))
    mel_spectrogram_end_time = time.time()
    mel_spectrogram_total_time = mel_spectrogram_end_time - mel_spectrogram_start_time

    if use_precompute:
        postfix_str = "audio_dsp_c_precompute"
        indicator_str = "Precompute"
    else:
        postfix_str = "audio_dsp_c_no_precompute"
        indicator_str = "No Precompute"

    save_plots(
        power_spectrum=power_spectrum,
        mel_spectrogram=mel_spectrogram,
        sample_rate=sample_rate,
        hop_length=hop_length,
        n_fft=n_fft,
        win_length=n_fft,
        target_directory=os.path.join(
            "examples", "_output", postfix_str),
        postfix_str=postfix_str,
    )
    print("Audio DSP ({}) - power spectrum: {}, mel spectrogram: {}".format(
        indicator_str,
        power_spectrum_total_time,
        mel_spectrogram_total_time,
    ))


def _main():
    # Load libraries
    import numpy as np
    from scipy.io import wavfile
    import librosa

    example_audio_filepath = "examples/example_audio.wav"

    # Gather system arguments
    library_binary_location = sys.argv[1]
    audio_python_interface_filepath = sys.argv[2]

    # Load the wave file as integer
    sample_rate, samples = wavfile.read(
        filename=example_audio_filepath)

    # Only use one channel (i.e mono)
    samples = samples[:, 0]

    # Load as int16 to mimic straight from the audio microphone
    if samples.dtype == np.int32 or samples.dtype == np.int64:
        samples = samples.astype(np.int16)
    assert (samples.dtype == np.int16)

    # Parameters are powers of 2 to take advantage of hardware
    n_fft = 1024
    n_mel = 64
    hop_length = int(n_fft / 4)
    scaling_factor_int16 = float(1.0 / np.iinfo(samples.dtype).max)

    print("sample_rate - {}, number of samples - {}, n_fft - {}, hop_length - {}, n_mel - {}, ".format(
        sample_rate,
        len(samples),
        n_fft,
        hop_length,
        n_mel,
    ))

    # Using audio_dsp's python interface (precompute and no precompute)
    # TODO: Update precomputed values
    # compute_and_plot_audio_dsp_c(
    #     library_binary_location=library_binary_location,
    #     audio_dsp_python_interface_filepath=audio_python_interface_filepath,
    #     samples=samples,
    #     sample_rate=sample_rate,
    #     scaling_factor=scaling_factor_int16,
    #     n_fft=n_fft,
    #     hop_length=hop_length,
    #     n_mel=n_mel,
    #     use_precompute=True,
    # )
    compute_and_plot_audio_dsp_c(
        library_binary_location=library_binary_location,
        audio_dsp_python_interface_filepath=audio_python_interface_filepath,
        samples=samples,
        sample_rate=sample_rate,
        scaling_factor=scaling_factor_int16,
        n_fft=n_fft,
        hop_length=hop_length,
        n_mel=n_mel,
        use_precompute=False,
    )

    # Librosa version
    power_spectrum_librosa_start_time = time.time()
    samples_float = samples.astype(np.float32) / scaling_factor_int16
    assert (samples_float.dtype == np.float32)
    stft_librosa = librosa.stft(
        y=samples_float,
        n_fft=n_fft,
        hop_length=hop_length,
        win_length=n_fft,
        # dtype=np.float32, # Gives a warning about casting complex values to real
    )
    power_spectrum_librosa = librosa.power_to_db(
        S=np.abs(stft_librosa) ** 2,
    )
    power_spectrum_librosa_end_time = time.time()
    power_spectrum_librosa_total_time = power_spectrum_librosa_end_time - power_spectrum_librosa_start_time

    mel_spectrogram_librosa_start_time = time.time()
    mel_spectrogram_librosa = librosa.feature.melspectrogram(
        S=power_spectrum_librosa,
        n_mels=n_mel,
        dtype=np.float32,
    )
    mel_spectrogram_librosa_end_time = time.time()
    mel_spectrogram_librosa_total_time = mel_spectrogram_librosa_end_time - mel_spectrogram_librosa_start_time

    # Save plots from librosa
    save_plots(
        power_spectrum=power_spectrum_librosa,
        mel_spectrogram=mel_spectrogram_librosa,
        sample_rate=sample_rate,
        hop_length=hop_length,
        n_fft=n_fft,
        win_length=n_fft,
        target_directory=os.path.join(
            "examples", "_output", "librosa"),
        postfix_str="librosa",
    )

    print("Librosa - power spectrum: {}, mel spectrogram: {}".format(
        power_spectrum_librosa_total_time,
        mel_spectrogram_librosa_total_time,
    ))


if __name__ == '__main__':
    _main()
