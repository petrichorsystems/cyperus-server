    filter_varslope_lowpass_params
      # USER
        float32_type[0]      - amplitude
        float32_type[1]      - slope
        float32_type[2]      - cutoff_frequency
      # INTERNAL
        float32_type[3]      - cutoff_frequency / (jackcli_samplerate / 2) # 0.0-1.0 mapped to nyquist
        float32_type[4..15]  - B[12] coefficients
        float32_type[16..23] - A[8] coefficients
        float32_type[24..39] - W0_FILT_STATES[16]
