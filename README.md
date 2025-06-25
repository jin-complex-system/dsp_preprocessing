# dsp_preprocessing
DSP algorithms for preprocessing

# Requirements

## Set up Python environment

1. Create and enter a virtual environment
```bash
python3 -m venv .venv

# Active virtual environment in Windows
.\.venv\Scripts\Activate.ps1

# Active virtual environment in Linux
source .venv/bin/activate
```
2. Install the required Python packages
```bash
pip3 install -r requirements.txt
```

# Build

## Building library
1. Use CMake to prepare:
```bash
mkdir build
cmake -B build -S . 
```
2. Inside `build`, build the libraries:
```bash
cd build
make -j16
```

# Python Scripts

## Run example
1. Inside your virtual environment, run the following:
```bash
<python_venv_interpreter> examples/compute_mel_spectrogram_example.py <path_to_library> audio_dsp\python_interface\audio_dsp_c.py
```
2. Observe the results in `examples/_output`

## Run tools to precompile headers
1. Update parameters inside [tools/parameters.py] to specify what values will be pre-computed
2. Inside your virtual environment, run the following:
```bash
<python_venv_interpreter> tools\generate_headers.py <path_to_library> audio_dsp\python_interface\audio_dsp_c.py
```

# Known issues
1. Precomputed mel spectrogram's last bin differs to raw computation; just discard the last bin

# TODO
- Write unit tests to check DSP-accelerated algorithms
- Check that padding in buffers for vectorised buffers is sufficient (might need to add 3 words)
