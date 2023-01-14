    oscillator_pulse_params
      # USER
        float32_type[0]      - frequency
        float32_type[1]      - pulse_width
        float32_type[2]      - mul
        float32_type[3]      - add        
      # INTERNAL
        float32_type[3]      - cutoff_frequency / (jackcli_samplerate / 2) # 0.0-1.0 mapped to nyquist
        float32_type[4..15]  - B[12] coefficients
        float32_type[16..23] - A[8] coefficients
        float32_type[24..39] - W0_FILT_STATES[16]
