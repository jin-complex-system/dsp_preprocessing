add_library(
        kissfft STATIC

        _kiss_fft_guts.h
        kiss_fft.h
        kiss_fft.c

        kfc.h
        kiss_fftnd.h
        kiss_fftr.h
        kiss_fftndr.h

        kfc.c
        kiss_fastfir.c
        kiss_fftnd.c
        kiss_fftndr.c
        kiss_fftr.c
)

target_include_directories(
        kissfft PUBLIC

        .
)
