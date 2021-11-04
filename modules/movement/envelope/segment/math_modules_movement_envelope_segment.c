/* math_modules_movement_envelope_segment.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2021 murray foster */


#include "math_modules_movement_envelope_segment.h"


enum {
    shape_Step,
    shape_Linear,
    shape_Exponential,
    shape_Sine,
    shape_Welch,
    shape_Curve,
    shape_Squared,
    shape_Cubed,
    shape_Hold,
    shape_Sustain = 9999
};

// called by nextSegment and check_gate:
// - counter: num samples to next segment
// - level: current envelope value
// - dur: if supplied and >= 0, stretch segment to last dur seconds (used in forced release)
int _init_segment(EnvGen* unit, int& counter, double& level, double dur = -1) {
    // Print("stage %d\n", unit->m_stage);
    // Print("initSegment\n");
    // out = unit->m_level;

  /* stageOffset is applied to mInBuf to get the envPtr,
       we probably don't need it when we switch to using env_gen_params_t */
  int stageOffset = (unit->m_stage << 2) + kEnvGen_nodeOffset;

  /* i suspect m_stage will be incremented each time before we run _init_segment() */

  
    if (stageOffset + 4 > unit->mNumInputs) {
        // oops.
        Print("envelope went past end of inputs.\n");
        ClearUnitOutputs(unit, 1);
        NodeEnd(&unit->mParent->mNode);
        return false;
    }

    float previous_end_level = unit->m_endLevel;
    if (unit->m_shape == shape_Hold)
        level = previous_end_level;
    float** envPtr = unit->mInBuf + stageOffset;
    double endLevel = *envPtr[0] * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias); // scale levels
    if (dur < 0)
        dur = *envPtr[1] * ZIN0(kEnvGen_timeScale);
    unit->m_shape = (int32)*envPtr[2];
    double curve = *envPtr[3];
    unit->m_endLevel = endLevel;

    counter = (int32)(dur * SAMPLERATE);
    counter = sc_max(1, counter);

    // Print("counter %d stageOffset %d   level %g   endLevel %g   dur %g   shape %d   curve %g\n", counter,
    // stageOffset, level, endLevel, dur, unit->m_shape, curve); Print("SAMPLERATE %g\n", SAMPLERATE);
    if (counter == 1)
        unit->m_shape = 1; // shape_Linear
    // Print("new counter = %d  shape = %d\n", counter, unit->m_shape);
    switch (unit->m_shape) {
    case shape_Step: {
        level = endLevel;
    } break;
    case shape_Hold: {
        level = previous_end_level;
    } break;
    case shape_Linear: {
        unit->m_grow = (endLevel - level) / counter;
        // Print("grow %g\n", unit->m_grow);
    } break;
    case shape_Exponential: {
        unit->m_grow = pow(endLevel / level, 1.0 / counter);
    } break;
    case shape_Sine: {
        double w = pi / counter;

        unit->m_a2 = (endLevel + level) * 0.5;
        unit->m_b1 = 2. * cos(w);
        unit->m_y1 = (endLevel - level) * 0.5;
        unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
        level = unit->m_a2 - unit->m_y1;
    } break;
    case shape_Welch: {
        double w = (pi * 0.5) / counter;

        unit->m_b1 = 2. * cos(w);

        if (endLevel >= level) {
            unit->m_a2 = level;
            unit->m_y1 = 0.;
            unit->m_y2 = -sin(w) * (endLevel - level);
        } else {
            unit->m_a2 = endLevel;
            unit->m_y1 = level - endLevel;
            unit->m_y2 = cos(w) * (level - endLevel);
        }
        level = unit->m_a2 + unit->m_y1;
    } break;
    case shape_Curve: {
        if (fabs(curve) < 0.001) {
            unit->m_shape = 1; // shape_Linear
            unit->m_grow = (endLevel - level) / counter;
        } else {
            double a1 = (endLevel - level) / (1.0 - exp(curve));
            unit->m_a2 = level + a1;
            unit->m_b1 = a1;
            unit->m_grow = exp(curve / counter);
        }
    } break;
    case shape_Squared: {
        unit->m_y1 = sqrt(level);
        unit->m_y2 = sqrt(endLevel);
        unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
    } break;
    case shape_Cubed: {
        unit->m_y1 = pow(level, 1.0 / 3.0); // 0.33333333);
        unit->m_y2 = pow(endLevel, 1.0 / 3.0);
        unit->m_grow = (unit->m_y2 - unit->m_y1) / counter;
    } break;
    };
    return true;
}

int _check_gate(EnvGen* unit, float prevGate, float gate, int& counter, double level, int counterOffset = 0) {
    if (prevGate <= 0.f && gate > 0.f) {
        unit->m_stage = -1;
        unit->m_released = false;
        unit->mDone = false;
        counter = counterOffset;
        return false;
    } else if (gate <= -1.f && prevGate > -1.f) {
        // forced release: jump to last segment overriding its duration
        double dur = -gate - 1.f;
        counter = (int32)(dur * SAMPLERATE);
        counter = modules_math_sc_max(1, counter) + counterOffset;
        unit->m_stage = static_cast<int>(ZIN0(kEnvGen_numStages) - 1);
        unit->m_released = true;
        EnvGen_initSegment(unit, counter, level, dur);
        return false;
    } else if (prevGate > 0.f && gate <= 0.f && unit->m_releaseNode >= 0 && !unit->m_released) {
        counter = counterOffset;
        unit->m_stage = unit->m_releaseNode - 1;
        unit->m_released = true;
        return false;
    }
    return 1;
}

int _check_gate_ar(EnvGen* unit, int i, float& prevGate, float*& gatein, int& nsmps, int& counter,
                                 double level) {
    const float gate = ZXP(gatein);
    const bool result = check_gate(unit, prevGate, gate, counter, level, i);
    if (!result) {
        --gatein;
        nsmps = i;
    }
    prevGate = gate;
    return result;
}

int _next_segment(EnvGen* unit, int& counter, double& level) {
    // Print("stage %d rel %d\n", unit->m_stage, (int)ZIN0(kEnvGen_releaseNode));
    int numstages = (int)ZIN0(kEnvGen_numStages);

    // Print("stage %d   numstages %d\n", unit->m_stage, numstages);
    if (unit->m_stage + 1 >= numstages) { // num stages
        // Print("stage+1 > num stages\n");
        counter = INT_MAX;
        unit->m_shape = 0;
        level = unit->m_endLevel;
        unit->mDone = true;
        int doneAction = (int)ZIN0(kEnvGen_doneAction);
        DoneAction(doneAction, unit);
    } else if (unit->m_stage == ENVGEN_NOT_STARTED) {
        counter = INT_MAX;
        return true;
    } else if (unit->m_stage + 1 == (int)ZIN0(kEnvGen_releaseNode) && !unit->m_released) { // sustain stage
        int loopNode = (int)ZIN0(kEnvGen_loopNode);
        if (loopNode >= 0 && loopNode < numstages) {
            unit->m_stage = loopNode;
            return EnvGen_initSegment(unit, counter, level);
        } else {
            counter = INT_MAX;
            unit->m_shape = shape_Sustain;
            level = unit->m_endLevel;
        }
        // Print("sustain\n");
    } else {
        unit->m_stage++;
        return EnvGen_initSegment(unit, counter, level);
    }

    return true;
}

void _perform(EnvGen* unit, float*& out, double& level, int& nsmps, GateCheck const& gateCheck) {
    switch (unit->m_shape) {
    case shape_Step:
    case shape_Hold: {
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
        }
    } break;
    case shape_Linear: {
        double grow = unit->m_grow;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            level += grow;
        }
    } break;
    case shape_Exponential: {
        double grow = unit->m_grow;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            level *= grow;
        }
    } break;
    case shape_Sine: {
        double a2 = unit->m_a2;
        double b1 = unit->m_b1;
        double y2 = unit->m_y2;
        double y1 = unit->m_y1;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            double y0 = b1 * y1 - y2;
            level = a2 - y0;
            y2 = y1;
            y1 = y0;
        }
        unit->m_y1 = y1;
        unit->m_y2 = y2;
    } break;
    case shape_Welch: {
        double a2 = unit->m_a2;
        double b1 = unit->m_b1;
        double y2 = unit->m_y2;
        double y1 = unit->m_y1;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            double y0 = b1 * y1 - y2;
            level = a2 + y0;
            y2 = y1;
            y1 = y0;
        }
        unit->m_y1 = y1;
        unit->m_y2 = y2;
    } break;
    case shape_Curve: {
        double a2 = unit->m_a2;
        double b1 = unit->m_b1;
        double grow = unit->m_grow;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            b1 *= grow;
            level = a2 - b1;
        }
        unit->m_b1 = b1;
    } break;
    case shape_Squared: {
        double grow = unit->m_grow;
        double y1 = unit->m_y1;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            y1 += grow;
            level = y1 * y1;
        }
        unit->m_y1 = y1;
    } break;
    case shape_Cubed: {
        double grow = unit->m_grow;
        double y1 = unit->m_y1;
        for (int i = 0; i < nsmps; ++i) {
            if (!gateCheck(i))
                break;
            ZXP(out) = level;
            y1 += grow;
            y1 = modules_math_sc_max(y1, 0);
            level = y1 * y1 * y1;
        }
        unit->m_y1 = y1;
    } break;
    case shape_Sustain: {
        for (int i = 0; i < nsmps; ++i) {
            if (CheckGateOnSustain) {
                if (gateCheck(i))
                    ZXP(out) = level;
            } else
                ZXP(out) = level;
        }
    } break;
    }
}

void _next_k(EnvGen* unit, int inNumSamples) {
    float gate = ZIN0(kEnvGen_gate);
    // Print("->EnvGen_next_k gate %g\n", gate);
    int counter = unit->m_counter;
    double level = unit->m_level;

    check_gate(unit, unit->m_prevGate, gate, counter, level);
    unit->m_prevGate = gate;

    // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
    // level0, numstages, releaseNode, loopNode,
    // [level, dur, shape, curve]

    if (counter <= 0) {
        bool success = EnvGen_nextSegment(unit, counter, level);
        if (!success)
            return;
    }

    float* out = ZOUT(0);
    _perform(unit, out, level, 1);

    // Print("x %d %d %d %g\n", unit->m_stage, counter, unit->m_shape, *out);
      cvvcc vv                                                                   unit->m_level = level;
    unit->m_counter = counter - 1;
}

void _next_aa(EnvGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* gatein = ZIN(kEnvGen_gate);
    int counter = unit->m_counter;
    double level = unit->m_level;
    float gate = unit->m_prevGate;
    int remain = inNumSamples;
    
    while (remain) {
        if (counter <= 0) {
            bool success = EnvGen_nextSegment(unit, counter, level);
            if (!success)
                return;
        }

        int nsmps = sc_min(remain, counter);
        _perform<true>(unit, out, level, nsmps,
                       [&](int i) { return check_gate_ar(unit, i, gate, gatein, nsmps, counter, level); });

        remain -= nsmps;
        counter -= nsmps;
    }
    unit->m_level = level;
    unit->m_counter = counter;
    unit->m_prevGate = gate;
    
}

extern
void math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters,
                                              float attack_rate,
                                              float decay_rate,
                                              float release_rate,
                                              float sustain_level,
                                              float target_ratio_a,
                                              float target_ratio_dr,
                                              float mul,
                                              float add) {
  
  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, releaseNode, loopNode,
  // [level, dur, shape, curve]
  
  unit->m_endLevel = unit->m_level = ZIN0(kEnvGen_initLevel) * ZIN0(kEnvGen_levelScale) + ZIN0(kEnvGen_levelBias);
  unit->m_counter = 0;
  unit->m_stage = ENVGEN_NOT_STARTED;
  unit->m_shape = shape_Hold;
  unit->m_prevGate = 0.f;
  unit->m_released = false;
  unit->m_releaseNode = (int)ZIN0(kEnvGen_releaseNode);
  
  float** envPtr = unit->mInBuf + kEnvGen_nodeOffset;
  const int initialShape = (int32)*envPtr[2];
  if (initialShape == shape_Hold)
    unit->m_level = *envPtr[0]; // we start at the end level;

  /* calculate first sample */
  EnvGen_next_k(unit, 1);

} /* math_modules_movement_envelope_segment_init */

extern
void math_modules_movement_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                              int gate,
                                              float attack_rate,
                                              float decay_rate,
                                              float release_rate,
                                              float sustain_level,
                                              float target_ratio_a,
                                              float target_ratio_dr,
                                              float mul,
                                              float add) {

}

extern
float math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float out = 0.0f;
  int fullrate = 1;

  
  
  return out;
}
