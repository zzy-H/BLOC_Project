/*
 * File: FOC_Mode0.c
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

#include "FOC_Mode0.h"
#define NumBitsPerChar                 8U

/* Exported data definition */

/* Definition for custom storage class: Struct */
CurrPID_type CurrPID = {
  /* Currki */
  35.0F,

  /* Currkp */
  0.17F
};

SpeedPID_type SpeedPID = {
  /* SpeedKi */
  0.0144F,

  /* SpeedKp */
  0.3389F
};

/* Block signals and states (default storage) */
HALL_DW HALL_rtDW;

/* External inputs (root inport signals with default storage) */
HALL_ExtU HALL_rtU;

/* External outputs (root outports fed by signals with default storage) */
HALL_ExtY HALL_rtY;

/* Real-time model */
static RT_MODEL rtM_;
RT_MODEL *const rtM = &rtM_;
static void rate_scheduler(void);
static real_T rtGetNaN(void);
static real32_T rtGetNaNF(void);
extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
static void rt_InitInfAndNaN(size_t realSize);
static boolean_T rtIsInf(real_T value);
static boolean_T rtIsInfF(real32_T value);
static boolean_T rtIsNaN(real_T value);
static boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;
static real_T rtGetInf(void);
static real32_T rtGetInfF(void);
static real_T rtGetMinusInf(void);
static real32_T rtGetMinusInfF(void);

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetNaNF(void)
{
  IEEESingle nanF = { { 0.0F } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
static void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
static boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
static boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
static boolean_T rtIsNaN(real_T value)
{
  boolean_T result = (boolean_T) 0;
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  if (bitsPerReal == 32U) {
    result = rtIsNaNF((real32_T)value);
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.fltVal = value;
    result = (boolean_T)((tmpVal.bitVal.words.wordH & 0x7FF00000) == 0x7FF00000 &&
                         ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                          (tmpVal.bitVal.words.wordL != 0) ));
  }

  return result;
}

/* Test if single-precision value is not a number */
static boolean_T rtIsNaNF(real32_T value)
{
  IEEESingle tmp;
  tmp.wordL.wordLreal = value;
  return (boolean_T)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                     (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real_T rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
static real32_T rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 *         This function updates active task flag for each subrate.
 *         The function is called at model base rate, hence the
 *         generated code self-manages all its subrates.
 */
static void rate_scheduler(void)
{
  /* Compute which subrates run during the next base time step.  Subrates
   * are an integer multiple of the base rate counter.  Therefore, the subtask
   * counter is reset when it reaches its limit (zero means run).
   */
  (rtM->Timing.TaskCounters.TID[1])++;
  if ((rtM->Timing.TaskCounters.TID[1]) > 9) {/* Sample time: [0.001s, 0.0s] */
    rtM->Timing.TaskCounters.TID[1] = 0;
  }
}

/* Model step function */
void FOC_Mode0_step(void)
{
  real_T rtb_Add;
  real_T rtb_Add1_p;
  real_T rtb_Add_l;
  real_T rtb_IntegralGain;
  real_T rtb_SignPreIntegrator;
  real_T rtb_SignPreSat;
  real_T rtb_SignPreSat_f;
  real_T rtb_Sum1;
  real32_T rtb_Add_e;
  real32_T rtb_Cos;
  real32_T rtb_Gain1_d;
  real32_T rtb_IProdOut;
  real32_T rtb_SignPreIntegrator_o;
  real32_T rtb_SignPreSat_n;
  real32_T rtb_Sin;

  /* Trigonometry: '<S2>/Cos' incorporates:
   *  Inport: '<Root>/theat'
   */
  rtb_Cos = cosf(HALL_rtU.theat);

  /* Sum: '<S5>/Add' incorporates:
   *  Gain: '<S5>/Gain'
   *  Inport: '<Root>/ia'
   *  Inport: '<Root>/ib'
   *  Inport: '<Root>/ic'
   *  Sum: '<S5>/Add1'
   */
  rtb_Add_e = 0.666666687F * HALL_rtU.ia - (HALL_rtU.ib + HALL_rtU.ic);

  /* Gain: '<S5>/Gain1' incorporates:
   *  Inport: '<Root>/ib'
   *  Inport: '<Root>/ic'
   *  Sum: '<S5>/Add2'
   */
  rtb_Gain1_d = (HALL_rtU.ib - HALL_rtU.ic) * 0.577350259F;

  /* Trigonometry: '<S2>/Sin' incorporates:
   *  Inport: '<Root>/theat'
   */
  rtb_Sin = sinf(HALL_rtU.theat);

  /* Sum: '<S8>/Sum' incorporates:
   *  Constant: '<S8>/Constant'
   *  Product: '<S6>/Product'
   *  Product: '<S6>/Product1'
   *  Sum: '<S6>/Add'
   */
  rtb_IProdOut = 0.0F - (rtb_Add_e * rtb_Cos + rtb_Gain1_d * rtb_Sin);

  /* Sum: '<S55>/Sum' incorporates:
   *  Constant: '<S8>/Constant6'
   *  DiscreteIntegrator: '<S46>/Integrator'
   *  Product: '<S51>/PProd Out'
   */
  rtb_SignPreSat_n = rtb_IProdOut * CurrPID.Currkp + HALL_rtDW.Integrator_DSTATE_o;

  /* Saturate: '<S53>/Saturation' */
  if (rtb_SignPreSat_n > 12.4707661F) {
    rtb_SignPreIntegrator_o = 12.4707661F;
  } else if (rtb_SignPreSat_n < -12.4707661F) {
    rtb_SignPreIntegrator_o = -12.4707661F;
  } else {
    rtb_SignPreIntegrator_o = rtb_SignPreSat_n;
  }

  /* End of Saturate: '<S53>/Saturation' */

  /* Sum: '<S3>/Sum' incorporates:
   *  Inport: '<Root>/SpeedFd'
   *  Inport: '<Root>/SpeedRef'
   */
  rtb_IntegralGain = HALL_rtU.SpeedRef - HALL_rtU.SpeedFd;

  /* Sum: '<S156>/Sum' incorporates:
   *  DiscreteIntegrator: '<S147>/Integrator'
   *  Gain: '<S152>/Proportional Gain'
   */
  rtb_SignPreSat = SpeedPID.SpeedKp * rtb_IntegralGain + HALL_rtDW.Integrator_DSTATE;

  /* Saturate: '<S154>/Saturation' */
  if (rtb_SignPreSat > 3.0) {
    rtb_Add = 3.0;
  } else if (rtb_SignPreSat < -3.0) {
    rtb_Add = -3.0;
  } else {
    rtb_Add = rtb_SignPreSat;
  }

  /* End of Saturate: '<S154>/Saturation' */

  /* Sum: '<S8>/Sum1' incorporates:
   *  Product: '<S6>/Product2'
   *  Product: '<S6>/Product3'
   *  Sum: '<S6>/Add1'
   */
  rtb_Sum1 = rtb_Add - (rtb_Gain1_d * rtb_Cos - rtb_Add_e * rtb_Sin);

  /* Sum: '<S105>/Sum' incorporates:
   *  Constant: '<S8>/Constant9'
   *  DiscreteIntegrator: '<S96>/Integrator'
   *  Product: '<S101>/PProd Out'
   */
  rtb_SignPreSat_f = rtb_Sum1 * CurrPID.Currkp + HALL_rtDW.Integrator_DSTATE_l;

  /* Saturate: '<S103>/Saturation' */
  if (rtb_SignPreSat_f > 12.470765814495918) {
    rtb_Add_l = 12.470765814495918;
  } else if (rtb_SignPreSat_f < -12.470765814495918) {
    rtb_Add_l = -12.470765814495918;
  } else {
    rtb_Add_l = rtb_SignPreSat_f;
  }

  /* End of Saturate: '<S103>/Saturation' */

  /* Sum: '<S4>/Add' incorporates:
   *  Product: '<S4>/Product'
   *  Product: '<S4>/Product1'
   */
  rtb_Add = rtb_SignPreIntegrator_o * rtb_Cos - rtb_Add_l * rtb_Sin;

  /* Gain: '<S9>/Gain' */
  rtb_Add1_p = -0.5 * rtb_Add;

  /* Gain: '<S9>/Gain1' incorporates:
   *  Product: '<S4>/Product2'
   *  Product: '<S4>/Product3'
   *  Sum: '<S4>/Add1'
   */
  rtb_SignPreIntegrator = (rtb_SignPreIntegrator_o * rtb_Sin + rtb_Add_l *
    rtb_Cos) * 0.8660254037844386;

  /* Sum: '<S9>/Add' */
  rtb_Add_l = rtb_Add1_p + rtb_SignPreIntegrator;

  /* Sum: '<S9>/Add1' */
  rtb_Add1_p -= rtb_SignPreIntegrator;

  /* Gain: '<S10>/Gain' incorporates:
   *  MinMax: '<S10>/Max'
   *  MinMax: '<S10>/Min'
   *  Sum: '<S10>/Sum'
   */
  rtb_SignPreIntegrator = (fmin(fmin(rtb_Add, rtb_Add_l), rtb_Add1_p) + fmax
    (fmax(rtb_Add, rtb_Add_l), rtb_Add1_p)) * -0.5;

  /* Outport: '<Root>/tABC' incorporates:
   *  Constant: '<S7>/Constant'
   *  Gain: '<S7>/Gain'
   *  Gain: '<S7>/Gain1'
   *  Inport: '<Root>/vbus'
   *  Product: '<S7>/Divide'
   *  Sum: '<S7>/Sum'
   *  Sum: '<S7>/Sum1'
   */
  HALL_rtY.tABC[0] = (-(rtb_SignPreIntegrator + rtb_Add) / HALL_rtU.vbus + 0.5) * 8000.0;
  HALL_rtY.tABC[1] = (-(rtb_SignPreIntegrator + rtb_Add_l) / HALL_rtU.vbus + 0.5) * 8000.0;
  HALL_rtY.tABC[2] = (-(rtb_SignPreIntegrator + rtb_Add1_p) / HALL_rtU.vbus + 0.5) *
    8000.0;

  /* Gain: '<S87>/ZeroGain' */
  rtb_Add_l = 0.0 * rtb_SignPreSat_f;

  /* DeadZone: '<S89>/DeadZone' */
  if (rtb_SignPreSat_f > 12.470765814495918) {
    rtb_SignPreSat_f -= 12.470765814495918;
  } else if (rtb_SignPreSat_f >= -12.470765814495918) {
    rtb_SignPreSat_f = 0.0;
  } else {
    rtb_SignPreSat_f -= -12.470765814495918;
  }

  /* End of DeadZone: '<S89>/DeadZone' */

  /* Product: '<S93>/IProd Out' incorporates:
   *  Constant: '<S8>/Constant10'
   */
  rtb_SignPreIntegrator = rtb_Sum1 * CurrPID.Currki;

  /* Gain: '<S138>/ZeroGain' */
  rtb_Sum1 = 0.0 * rtb_SignPreSat;

  /* DeadZone: '<S140>/DeadZone' */
  if (rtb_SignPreSat > 3.0) {
    rtb_SignPreSat -= 3.0;
  } else if (rtb_SignPreSat >= -3.0) {
    rtb_SignPreSat = 0.0;
  } else {
    rtb_SignPreSat -= -3.0;
  }

  /* End of DeadZone: '<S140>/DeadZone' */

  /* Gain: '<S144>/Integral Gain' */
  rtb_IntegralGain *= SpeedPID.SpeedKi;

  /* Gain: '<S37>/ZeroGain' */
  rtb_Cos = 0.0F * rtb_SignPreSat_n;

  /* DeadZone: '<S39>/DeadZone' */
  if (rtb_SignPreSat_n > 12.4707661F) {
    rtb_SignPreSat_n -= 12.4707661F;
  } else if (rtb_SignPreSat_n >= -12.4707661F) {
    rtb_SignPreSat_n = 0.0F;
  } else {
    rtb_SignPreSat_n -= -12.4707661F;
  }

  /* End of DeadZone: '<S39>/DeadZone' */

  /* Product: '<S43>/IProd Out' incorporates:
   *  Constant: '<S8>/Constant7'
   */
  rtb_IProdOut *= CurrPID.Currki;

  /* Signum: '<S37>/SignPreSat' */
  if (rtb_SignPreSat_n < 0.0F) {
    rtb_Add_e = -1.0F;
  } else if (rtb_SignPreSat_n > 0.0F) {
    rtb_Add_e = 1.0F;
  } else if (rtb_SignPreSat_n == 0.0F) {
    rtb_Add_e = 0.0F;
  } else {
    rtb_Add_e = (rtNaNF);
  }

  /* End of Signum: '<S37>/SignPreSat' */

  /* Signum: '<S37>/SignPreIntegrator' */
  if (rtb_IProdOut < 0.0F) {
    rtb_Gain1_d = -1.0F;
  } else if (rtb_IProdOut > 0.0F) {
    rtb_Gain1_d = 1.0F;
  } else if (rtb_IProdOut == 0.0F) {
    rtb_Gain1_d = 0.0F;
  } else {
    rtb_Gain1_d = (rtNaNF);
  }

  /* End of Signum: '<S37>/SignPreIntegrator' */

  /* Switch: '<S37>/Switch' incorporates:
   *  Constant: '<S37>/Constant1'
   *  DataTypeConversion: '<S37>/DataTypeConv1'
   *  DataTypeConversion: '<S37>/DataTypeConv2'
   *  Logic: '<S37>/AND3'
   *  RelationalOperator: '<S37>/Equal1'
   *  RelationalOperator: '<S37>/NotEqual'
   */
  if ((rtb_Cos != rtb_SignPreSat_n) && ((int8_T)rtb_Add_e == (int8_T)rtb_Gain1_d))
  {
    rtb_IProdOut = 0.0F;
  }

  /* End of Switch: '<S37>/Switch' */

  /* Update for DiscreteIntegrator: '<S46>/Integrator' */
  HALL_rtDW.Integrator_DSTATE_o += 0.0001F * rtb_IProdOut;

  /* Signum: '<S138>/SignPreSat' */
  if (rtb_SignPreSat < 0.0) {
    rtb_Add = -1.0;
  } else if (rtb_SignPreSat > 0.0) {
    rtb_Add = 1.0;
  } else if (rtb_SignPreSat == 0.0) {
    rtb_Add = 0.0;
  } else {
    rtb_Add = (rtNaN);
  }

  /* End of Signum: '<S138>/SignPreSat' */

  /* Signum: '<S138>/SignPreIntegrator' */
  if (rtb_IntegralGain < 0.0) {
    rtb_Add1_p = -1.0;
  } else if (rtb_IntegralGain > 0.0) {
    rtb_Add1_p = 1.0;
  } else if (rtb_IntegralGain == 0.0) {
    rtb_Add1_p = 0.0;
  } else {
    rtb_Add1_p = (rtNaN);
  }

  /* End of Signum: '<S138>/SignPreIntegrator' */

  /* Switch: '<S138>/Switch' incorporates:
   *  Constant: '<S138>/Constant1'
   *  DataTypeConversion: '<S138>/DataTypeConv1'
   *  DataTypeConversion: '<S138>/DataTypeConv2'
   *  Logic: '<S138>/AND3'
   *  RelationalOperator: '<S138>/Equal1'
   *  RelationalOperator: '<S138>/NotEqual'
   */
  if ((rtb_Sum1 != rtb_SignPreSat) && ((int8_T)rtb_Add == (int8_T)rtb_Add1_p)) {
    rtb_IntegralGain = 0.0;
  }

  /* End of Switch: '<S138>/Switch' */

  /* Update for DiscreteIntegrator: '<S147>/Integrator' */
  HALL_rtDW.Integrator_DSTATE += 0.0001 * rtb_IntegralGain;

  /* Signum: '<S87>/SignPreSat' */
  if (rtb_SignPreSat_f < 0.0) {
    rtb_IntegralGain = -1.0;
  } else if (rtb_SignPreSat_f > 0.0) {
    rtb_IntegralGain = 1.0;
  } else if (rtb_SignPreSat_f == 0.0) {
    rtb_IntegralGain = 0.0;
  } else {
    rtb_IntegralGain = (rtNaN);
  }

  /* End of Signum: '<S87>/SignPreSat' */

  /* Signum: '<S87>/SignPreIntegrator' */
  if (rtb_SignPreIntegrator < 0.0) {
    rtb_SignPreSat = -1.0;
  } else if (rtb_SignPreIntegrator > 0.0) {
    rtb_SignPreSat = 1.0;
  } else if (rtb_SignPreIntegrator == 0.0) {
    rtb_SignPreSat = 0.0;
  } else {
    rtb_SignPreSat = (rtNaN);
  }

  /* End of Signum: '<S87>/SignPreIntegrator' */

  /* Switch: '<S87>/Switch' incorporates:
   *  Constant: '<S87>/Constant1'
   *  DataTypeConversion: '<S87>/DataTypeConv1'
   *  DataTypeConversion: '<S87>/DataTypeConv2'
   *  Logic: '<S87>/AND3'
   *  RelationalOperator: '<S87>/Equal1'
   *  RelationalOperator: '<S87>/NotEqual'
   */
  if ((rtb_Add_l != rtb_SignPreSat_f) && ((int8_T)rtb_IntegralGain == (int8_T)
       rtb_SignPreSat)) {
    rtb_SignPreIntegrator = 0.0;
  }

  /* End of Switch: '<S87>/Switch' */

  /* Update for DiscreteIntegrator: '<S96>/Integrator' */
  HALL_rtDW.Integrator_DSTATE_l += 0.0001 * rtb_SignPreIntegrator;
  rate_scheduler();
}

/* Model initialize function */
void FOC_Mode0_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
