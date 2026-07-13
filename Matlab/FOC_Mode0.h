/*
 * File: FOC_Mode0.h
 *
 * Code generated for Simulink model 'FOC_Mode0'.
 *
 * Model version                  : 1.22
 * Simulink Coder version         : 9.6 (R2021b) 14-May-2021
 * C/C++ source code generated on : Wed Jul  8 11:18:12 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_FOC_Mode0_h_
#define RTW_HEADER_FOC_Mode0_h_
#include <stddef.h>
#include <math.h>
#ifndef FOC_Mode0_COMMON_INCLUDES_
#define FOC_Mode0_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* FOC_Mode0_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

#define FOC_Mode0_M                    (rtM)

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real_T Integrator_DSTATE;            /* '<S147>/Integrator' */
  real_T Integrator_DSTATE_l;          /* '<S96>/Integrator' */
  real32_T Integrator_DSTATE_o;        /* '<S46>/Integrator' */
} HALL_DW;

/* External inputs (root inport signals with default storage) */
typedef struct {
  real32_T ia;                         /* '<Root>/ia' */
  real32_T ib;                         /* '<Root>/ib' */
  real32_T ic;                         /* '<Root>/ic' */
  real32_T vbus;                       /* '<Root>/vbus' */
  real32_T theat;                      /* '<Root>/theat' */
  real_T SpeedRef;                     /* '<Root>/SpeedRef' */
  real32_T SpeedFd;                    /* '<Root>/SpeedFd' */
} HALL_ExtU;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T tABC[3];                      /* '<Root>/tABC' */
} HALL_ExtY;

/* Type definition for custom storage class: Struct */
typedef struct CurrPID_tag {
  real32_T Currki;                     /* Referenced by:
                                        * '<S8>/Constant10'
                                        * '<S8>/Constant7'
                                        */
  real32_T Currkp;                     /* Referenced by:
                                        * '<S8>/Constant6'
                                        * '<S8>/Constant9'
                                        */
} CurrPID_type;

typedef struct SpeedPID_tag {
  real32_T SpeedKi;                  /* Referenced by: '<S144>/Integral Gain' */
  real32_T SpeedKp;              /* Referenced by: '<S152>/Proportional Gain' */
} SpeedPID_type;

/* Real-time Model Data Structure */
struct tag_RTM {
  const char_T * volatile errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    struct {
      uint8_T TID[2];
    } TaskCounters;
  } Timing;
};

/* Block signals and states (default storage) */
extern HALL_DW HALL_rtDW;

/* External inputs (root inport signals with default storage) */
extern HALL_ExtU HALL_rtU;

/* External outputs (root outports fed by signals with default storage) */
extern HALL_ExtY HALL_rtY;

/* Model entry point functions */
extern void FOC_Mode0_initialize(void);
extern void FOC_Mode0_step(void);

/* Exported data declaration */

/* Declaration for custom storage class: Struct */
extern CurrPID_type CurrPID;
extern SpeedPID_type SpeedPID;

/* Real-time Model object */
extern RT_MODEL *const rtM;

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S2>/Data Type Conversion' : Eliminate redundant data type conversion
 * Block '<S2>/Data Type Conversion1' : Eliminate redundant data type conversion
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
 * hilite_system('FOC_MODE/FOC_Mode')    - opens subsystem FOC_MODE/FOC_Mode
 * hilite_system('FOC_MODE/FOC_Mode/Kp') - opens and selects block Kp
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'FOC_MODE'
 * '<S1>'   : 'FOC_MODE/FOC_Mode'
 * '<S2>'   : 'FOC_MODE/FOC_Mode/Currloop'
 * '<S3>'   : 'FOC_MODE/FOC_Mode/Speedloop'
 * '<S4>'   : 'FOC_MODE/FOC_Mode/Currloop/AntiPark'
 * '<S5>'   : 'FOC_MODE/FOC_Mode/Currloop/Clark1'
 * '<S6>'   : 'FOC_MODE/FOC_Mode/Currloop/Park1'
 * '<S7>'   : 'FOC_MODE/FOC_Mode/Currloop/SVPWM'
 * '<S8>'   : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller'
 * '<S9>'   : 'FOC_MODE/FOC_Mode/Currloop/SVPWM/AntiClark'
 * '<S10>'  : 'FOC_MODE/FOC_Mode/Currloop/SVPWM/ei_t'
 * '<S11>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller'
 * '<S12>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1'
 * '<S13>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Anti-windup'
 * '<S14>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/D Gain'
 * '<S15>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Filter'
 * '<S16>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Filter ICs'
 * '<S17>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/I Gain'
 * '<S18>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Ideal P Gain'
 * '<S19>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Ideal P Gain Fdbk'
 * '<S20>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Integrator'
 * '<S21>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Integrator ICs'
 * '<S22>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/N Copy'
 * '<S23>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/N Gain'
 * '<S24>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/P Copy'
 * '<S25>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Parallel P Gain'
 * '<S26>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Reset Signal'
 * '<S27>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Saturation'
 * '<S28>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Saturation Fdbk'
 * '<S29>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Sum'
 * '<S30>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Sum Fdbk'
 * '<S31>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tracking Mode'
 * '<S32>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tracking Mode Sum'
 * '<S33>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tsamp - Integral'
 * '<S34>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tsamp - Ngain'
 * '<S35>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/postSat Signal'
 * '<S36>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/preSat Signal'
 * '<S37>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Anti-windup/Disc. Clamping Parallel'
 * '<S38>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
 * '<S39>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
 * '<S40>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/D Gain/Disabled'
 * '<S41>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Filter/Disabled'
 * '<S42>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Filter ICs/Disabled'
 * '<S43>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/I Gain/External Parameters'
 * '<S44>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Ideal P Gain/Passthrough'
 * '<S45>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S46>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Integrator/Discrete'
 * '<S47>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Integrator ICs/Internal IC'
 * '<S48>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S49>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/N Gain/Disabled'
 * '<S50>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/P Copy/Disabled'
 * '<S51>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Parallel P Gain/External Parameters'
 * '<S52>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Reset Signal/Disabled'
 * '<S53>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Saturation/Enabled'
 * '<S54>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Saturation Fdbk/Disabled'
 * '<S55>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Sum/Sum_PI'
 * '<S56>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Sum Fdbk/Disabled'
 * '<S57>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tracking Mode/Disabled'
 * '<S58>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S59>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tsamp - Integral/Passthrough'
 * '<S60>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S61>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/postSat Signal/Forward_Path'
 * '<S62>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller/preSat Signal/Forward_Path'
 * '<S63>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Anti-windup'
 * '<S64>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/D Gain'
 * '<S65>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Filter'
 * '<S66>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Filter ICs'
 * '<S67>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/I Gain'
 * '<S68>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Ideal P Gain'
 * '<S69>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Ideal P Gain Fdbk'
 * '<S70>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Integrator'
 * '<S71>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Integrator ICs'
 * '<S72>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/N Copy'
 * '<S73>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/N Gain'
 * '<S74>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/P Copy'
 * '<S75>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Parallel P Gain'
 * '<S76>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Reset Signal'
 * '<S77>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Saturation'
 * '<S78>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Saturation Fdbk'
 * '<S79>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Sum'
 * '<S80>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Sum Fdbk'
 * '<S81>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tracking Mode'
 * '<S82>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tracking Mode Sum'
 * '<S83>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tsamp - Integral'
 * '<S84>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tsamp - Ngain'
 * '<S85>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/postSat Signal'
 * '<S86>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/preSat Signal'
 * '<S87>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Anti-windup/Disc. Clamping Parallel'
 * '<S88>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Anti-windup/Disc. Clamping Parallel/Dead Zone'
 * '<S89>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
 * '<S90>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/D Gain/Disabled'
 * '<S91>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Filter/Disabled'
 * '<S92>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Filter ICs/Disabled'
 * '<S93>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/I Gain/External Parameters'
 * '<S94>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Ideal P Gain/Passthrough'
 * '<S95>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Ideal P Gain Fdbk/Disabled'
 * '<S96>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Integrator/Discrete'
 * '<S97>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Integrator ICs/Internal IC'
 * '<S98>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/N Copy/Disabled wSignal Specification'
 * '<S99>'  : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/N Gain/Disabled'
 * '<S100>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/P Copy/Disabled'
 * '<S101>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Parallel P Gain/External Parameters'
 * '<S102>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Reset Signal/Disabled'
 * '<S103>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Saturation/Enabled'
 * '<S104>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Saturation Fdbk/Disabled'
 * '<S105>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Sum/Sum_PI'
 * '<S106>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Sum Fdbk/Disabled'
 * '<S107>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tracking Mode/Disabled'
 * '<S108>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tracking Mode Sum/Passthrough'
 * '<S109>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tsamp - Integral/Passthrough'
 * '<S110>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/Tsamp - Ngain/Passthrough'
 * '<S111>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/postSat Signal/Forward_Path'
 * '<S112>' : 'FOC_MODE/FOC_Mode/Currloop/idq_Controller/PID Controller1/preSat Signal/Forward_Path'
 * '<S113>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller'
 * '<S114>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Anti-windup'
 * '<S115>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/D Gain'
 * '<S116>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Filter'
 * '<S117>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Filter ICs'
 * '<S118>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/I Gain'
 * '<S119>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Ideal P Gain'
 * '<S120>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Ideal P Gain Fdbk'
 * '<S121>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Integrator'
 * '<S122>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Integrator ICs'
 * '<S123>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/N Copy'
 * '<S124>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/N Gain'
 * '<S125>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/P Copy'
 * '<S126>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Parallel P Gain'
 * '<S127>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Reset Signal'
 * '<S128>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Saturation'
 * '<S129>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Saturation Fdbk'
 * '<S130>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Sum'
 * '<S131>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Sum Fdbk'
 * '<S132>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tracking Mode'
 * '<S133>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tracking Mode Sum'
 * '<S134>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tsamp - Integral'
 * '<S135>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tsamp - Ngain'
 * '<S136>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/postSat Signal'
 * '<S137>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/preSat Signal'
 * '<S138>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Anti-windup/Disc. Clamping Parallel'
 * '<S139>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone'
 * '<S140>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Anti-windup/Disc. Clamping Parallel/Dead Zone/Enabled'
 * '<S141>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/D Gain/Disabled'
 * '<S142>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Filter/Disabled'
 * '<S143>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Filter ICs/Disabled'
 * '<S144>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/I Gain/Internal Parameters'
 * '<S145>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Ideal P Gain/Passthrough'
 * '<S146>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S147>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Integrator/Discrete'
 * '<S148>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Integrator ICs/Internal IC'
 * '<S149>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/N Copy/Disabled wSignal Specification'
 * '<S150>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/N Gain/Disabled'
 * '<S151>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/P Copy/Disabled'
 * '<S152>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Parallel P Gain/Internal Parameters'
 * '<S153>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Reset Signal/Disabled'
 * '<S154>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Saturation/Enabled'
 * '<S155>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Saturation Fdbk/Disabled'
 * '<S156>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Sum/Sum_PI'
 * '<S157>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Sum Fdbk/Disabled'
 * '<S158>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tracking Mode/Disabled'
 * '<S159>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tracking Mode Sum/Passthrough'
 * '<S160>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tsamp - Integral/Passthrough'
 * '<S161>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/Tsamp - Ngain/Passthrough'
 * '<S162>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/postSat Signal/Forward_Path'
 * '<S163>' : 'FOC_MODE/FOC_Mode/Speedloop/PID Controller/preSat Signal/Forward_Path'
 */
#endif                                 /* RTW_HEADER_FOC_Mode0_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
