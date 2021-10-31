    envelope_adsr_params
      # USER
        int8_type[0] = gate;
        float32_type[0] = attack_rate; 
        float32_type[1] = decay_rate;
        float32_type[2] = release_rate;
        float32_type[3] = sustain_level;
        float32_type[4] = target_ratio_a;
        float32_type[5] = target_ratio_dr;
        float32_type[6] = mul;
        float32_type[7] = add;      
      # INTERNAL
        int8_type[1] = state;
        float32_type[8] = attack_coeff;
        float32_type[9] = decay_coeff;
        float32_type[10] = release_coeff;
        float32_type[11] = attack_base;
        float32_type[12] = decay_base;
        float32_type[13] = release_base;
        float32_type[14] = last_output_value;
