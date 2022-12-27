delay_simple_params
  # USER
  float32_type[0] = frequency;
  float32_type[1] = gain; 
  float32_type[2] = reset; 
  float32_type[3] = mul;
  float32_type[4] = add;

  # INTERNAL
  float32_type[4] = 0.0f; /* m_freq */
  float32_type[5] = gain; /* m_k */

  double_type[0] = 0.0f; /* m_b0 - resonant freq*/
  double_type[1] = 0.0f; /* m_a1 - corresponding value */

  double_type[2] = 0.0f; /* m_s1 - internal parameters, 1st order filter states */
  double_type[3] = 0.0f; /* m_s2 */
  double_type[4] = 0.0f; /* m_s3 */
  double_type[5] = 0.0f; /* m_s4 */
