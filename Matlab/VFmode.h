/*
 * File: VFmode.h
 *
 * Code generated for Simulink model 'VFmode'.
 *
 * Model version                  : 1.11
 * Simulink Coder version         : 9.6 (R2021b) 14-May-2021
 * C/C++ source code generated on : Tue Jul  7 13:08:42 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_VFmode_h_
#define RTW_HEADER_VFmode_h_
#include <math.h>
#ifndef VFmode_COMMON_INCLUDES_
#define VFmode_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* VFmode_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real32_T UnitDelay_DSTATE;           /* '<S3>/Unit Delay' */
} VF_DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T ud;                         /* '<Root>/ud' */
  real32_T uq;                         /* '<Root>/uq' */
  real32_T Freq;                       /* '<Root>/Freq' */
  real32_T Vbus;                       /* '<Root>/Vbus' */
} VF_ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real32_T tABC[3];                    /* '<Root>/tABC' */
} VF_ExtY;

/* Block signals and states (default storage) */
extern VF_DW VF_rtDW;

/* External inputs (root inport signals with default storage) */
extern VF_ExtU VF_rtU;

/* External outputs (root outports fed by signals with default storage) */
extern VF_ExtY VF_rtY;

/* Model entry point functions */
extern void VFmode_initialize(void);
extern void VFmode_step(void);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S1>/Data Type Conversion' : Eliminate redundant data type conversion
 * Block '<S1>/Data Type Conversion1' : Eliminate redundant data type conversion
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Note that this particular code originates from a subsystem build,
 * and has its own system numbers different from the parent model.
 * Refer to the system hierarchy for this subsystem below, and use the
 * MATLAB hilite_system command to trace the generated code back
 * to the parent model.  For example,
 *
 * hilite_system('FOC_MODE/VFmode')    - opens subsystem FOC_MODE/VFmode
 * hilite_system('FOC_MODE/VFmode/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'FOC_MODE'
 * '<S1>'   : 'FOC_MODE/VFmode'
 * '<S2>'   : 'FOC_MODE/VFmode/AntiPark'
 * '<S3>'   : 'FOC_MODE/VFmode/FreqGenerator'
 * '<S4>'   : 'FOC_MODE/VFmode/SVPWM'
 * '<S5>'   : 'FOC_MODE/VFmode/FreqGenerator/RangeLimit'
 * '<S6>'   : 'FOC_MODE/VFmode/FreqGenerator/RangeLimit/If Action Subsystem'
 * '<S7>'   : 'FOC_MODE/VFmode/FreqGenerator/RangeLimit/If Action Subsystem1'
 * '<S8>'   : 'FOC_MODE/VFmode/FreqGenerator/RangeLimit/If Action Subsystem2'
 * '<S9>'   : 'FOC_MODE/VFmode/SVPWM/AntiClark'
 * '<S10>'  : 'FOC_MODE/VFmode/SVPWM/ei_t'
 */
#endif                                 /* RTW_HEADER_VFmode_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
