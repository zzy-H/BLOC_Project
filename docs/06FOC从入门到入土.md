# FOC 从入门到入土

---

## 0. 电机参数（实测值）

当前项目使用的电机参数（实测，2026-07-07 确认）：

| 参数 | 值 | 说明 |
|------|-----|------|
| Rs | 6.97 Ω | 相电阻（万用表测量值） |
| L | 0.00535 H | 相电感 |
| flux | 0.016884 Wb | 永磁磁链 |
| Pn | 2 | 极对数 |

> **注意：** 之前使用的另一组参数（Rs=0.02Ω, L=0.0017H, flux=0.2205Wb, Pn=4）是另一台电机的，不要混用。

**电流环 PI 参数计算（基于上述电机参数）：**

```
目标带宽 ωc = 500 rad/s（保守值，MATLAB 模型默认）
Kp = L × ωc = 0.00535 × 500 = 2.675
Ki = Rs × ωc = 6.97 × 500 = 3485
```

MATLAB 模型原始值 CurrKp=0.026、CurrKi=35 对应的带宽约 5 rad/s，远小于正常值。

**FOC_Mode0 模型（有感 FOC，2026-07-08 生成）PI 参数：**

PI = **Proportional（比例） + Integral（积分）**

| 参数 | 值 | 作用 | 说明 |
|------|-----|------|------|
| Currkp | 0.17 | 电流环 P 增益 | 电流误差 × Kp = 输出电压。越大响应越快，太大会震荡。 |
| Currki | 35.0 | 电流环 I 增益 | 消除电流静差。误差累积 × Ki 逐步修正。太大→积分饱和。 |
| SpeedKp | 0.3389 | 速度环 P 增益 | 速度误差 × Kp = 目标 Iq。越大速度响应越快。 |
| SpeedKi | 0.0144 | 速度环 I 增益 | 消除速度静差。让实际转速稳定到目标转速。 |

**参数之间的关系：**

```
速度环：目标转速 ─→ 误差 ─→ (Kp×误差 + Ki×∫误差) ─→ 目标 Iq ─→ 电流环 ─→ 电机
                                                                            ↑
电流环：目标 Iq  ─→ 误差 ─→ (Kp×误差 + Ki×∫误差) ─→ Vq ─→ 电机 ←─── 实际电流
                        ↑                              ↑
                  反馈实际 Iq                      反馈实际转速
```

**怎么调：**

```
电机响应慢 → 增大 Currkp（电流环）或 SpeedKp（速度环）
电机震荡   → 减小 Currkp 或 SpeedKp
有静差     → 增大 Currki 或 SpeedKi
积分饱和   → 减小 Currki 或加抗饱和

先调电流环（Currkp），再调速度环（SpeedKp）。
电流环调稳了才能调速度环。
```

**电机 KV 值（估算）：**

KV 值 = 空载转速（RPM）÷ 电压（V）-->每 1V 电压下电机的**空载转速**

从电机参数推算（24V 母线，Pn=2）：

```
额定转速 ≈ 3000 RPM（估算）
KV ≈ 3000 / 24 ≈ 125 KV
```

> **注意：** KV 值是电机的固有属性，和 FOC 算法无关。上面的 125KV 是估算值，实际需要测空载转速才能确定。KV 值越高，同样电压下转速越快，但力矩越小。

**FOC_Mode0 输入接口（HALL_rtU）：**

| 字段 | 类型 | 说明 |
|------|------|------|
| ia | float32 | U 相电流 |
| ib | float32 | V 相电流 |
| ic | float32 | W 相电流 |
| vbus | float32 | 母线电压 |
| theat | float32 | **霍尔角度（直接输入，不需要 theta_enable）** |
| SpeedRef | float64 | 目标速度 |
| SpeedFd | float64 | 速度反馈 |

**FOC_Mode0 输出接口（HALL_rtY）：**
- `tABC[0]` = U 相 CCR
- `tABC[1]` = V 相 CCR
- `tABC[2]` = W 相 CCR

如需调参，应以此公式为基准。

---

## 1. FOC 概述

### 什么是 FOC

FOC（Field-Oriented Control）为**磁场定向控制**，又称矢量控制，是目前 BLDC 和 PMSM 高效控制的最佳选择。可以精确控制磁场大小与方向，使电机转矩平稳、噪声小、效率高、动态响应快。

### 六步换向的问题

每 60° 换一次相，一次只两相通电，换相瞬间电流突变 → 扭矩脉动 → 噪音、振动。

```
六步换向电流：┌───┐           ┌───┐
            │   │           │   │
            └───┴───┴───┴───┴───┴───
               ↑ 换相瞬间电流突变
```

**FOC 的目标：** 三相电流都是平滑正弦波，任何时候三相都有电。

```
FOC 三相电流：U ~~~~~~~~  V  ~~~~~~~~  W    ~~~~~~~~
              叠加 → 恒定旋转磁场 → 扭矩恒定
```

### 有感 vs 无感

| | 有感 | 无感 |
|--|------|------|
| 角度来源 | 霍尔/编码器直读 | 观测器估算（基于反电动势） |
| 低速性能 | ✅ 好 | ❌ 差 |
| 启动 | ✅ 直接启动 | ❌ 需要强拖再切闭环 |
| 硬件成本 | ❌ 多传感器 | ✅ 省掉 |

### 速度环 vs 电流环

速度输入和速度闭环是**可选**的：
- **速度闭环**：外环调速度，内环调电流
- **扭矩闭环**：只调电流，不控速度

```
                          外环：速度 PI 环         内环：电流 PI 环
目标速度 ──→ 速度误差 → PI ──Iq_ref──→ 电流误差 → PI → Vd,Vq ──→ 电机
实际速度 ←───────────────────── 实际 Iq ←────────────────────────
```

电流环 10~20kHz，速度环 1~2kHz。电流变化比速度快得多，内环必须快。

---

## 2. 坐标变换

FOC 的核心：通过坐标变换把"控制三相正弦波"变成"控制两个直流分量 Id、Iq"。

```
                   Clarke               Park
  三相电流 ──────────→ 两相静止 αβ ────────→ 旋转 dq
  (Ia, Ib, Ic)           (Iα, Iβ)              (Id, Iq)
```

### Clarke 变换：三相（ABC）→ 两相静止（αβ）

矩阵形式

![image-20260703100836372](E:\STM32\BLOC_PMSM\06FOC从入门到入土\image-20260703100836372.png)



![image-20260703085630011](E:\STM32\BLOC_PMSM\06FOC从入门到入土\image-20260703085630011.png)

三相电流表达式（A 相为基准，B 滞后 120°，C 超前 120°）：

$$
\begin{cases}
i_A = I_m \cos\omega t \\
i_B = I_m \cos(\omega t - 2\pi/3) \\
i_C = I_m \cos(\omega t + 2\pi/3)
\end{cases}
$$

三相平衡电流的关键性质：$i_A + i_B + i_C = 0$。

Clarke 变换公式：
$$
\begin{aligned}
i_\alpha &= \frac{2}{3}\left(i_A - \frac12 i_B - \frac12 i_C\right) \\
i_\beta &= \frac{1}{\sqrt3}(i_B - i_C)
\end{aligned}
$$

系数 $\frac{2}{3}$ 的推导（恒幅值约束）：

$$
\begin{aligned}
i_\alpha &= \frac{N_3}{N_2}(i_A - \frac12 i_B - \frac12 i_C) \\
        &= \frac{N_3}{N_2} \cdot \frac32 i_A = \frac{N_3}{N_2} \cdot \frac32 I_m \cos\omega t
\end{aligned}
$$

令变换后幅值仍为 $I_m$：
$$
\displaystyle\frac{N_3}{N_2} \cdot \frac32 = 1 \implies \boxed{\frac{N_3}{N_2} = \frac{2}{3}}
$$
**恒幅值 vs 恒功率：**
- 恒幅值（系数 2/3）：幅值不变，功率变为 1.5 倍。ST MCSDK 默认用此方案
- 恒功率（系数 $\sqrt{2/3}$）：功率不变，幅值变。调参不直观

### 逆 Clark 变换（反 Clark）

矩阵形式

![image-20260703100900901](E:\STM32\BLOC_PMSM\06FOC从入门到入土\image-20260703100900901.png)

从 αβ 回到三相电压：

$$
\begin{cases}
V_A = V_\alpha \\
V_B = -\dfrac12 V_\alpha + \dfrac{\sqrt3}{2} V_\beta \\
V_C = -\dfrac12 V_\alpha - \dfrac{\sqrt3}{2} V_\beta
\end{cases}
$$

实际 FOC 中更常用 SVPWM 直接合成电压，逆 Clark 主要用于理论调试。

### Park 变换：静止（αβ）→ 旋转（dq）

Clark 变换后仍是正弦交变，不利于 PI 控制。Park 变换把坐标系**跟转子一起转**，把正弦量变成直流量。

```
                 q 轴（交轴）↑
                     │    ┌──┐
                     │    │N │
                     ├────┴──┴──→ d 轴（直轴）
                     │    │S │
```

Park 变换公式（给定转子角度 θ）：

$$
\begin{cases}
i_d = i_\alpha \cos\theta + i_\beta \sin\theta \\
i_q = -i_\alpha \sin\theta + i_\beta \cos\theta
\end{cases}
$$

### 逆 Park 变换（反 Park）

PI 控制器输出的是 dq 电压，SVPWM 需要 αβ 电压：

$$
\begin{cases}
V_\alpha = V_d \cos\theta - V_q \sin\theta \\
V_\beta = V_d \sin\theta + V_q \cos\theta
\end{cases}
$$

正 Park 和逆 Park 的符号差异：

```
正 Park（电流方向）：      id =  iα·cosθ + iβ·sinθ
                           iq = -iα·sinθ + iβ·cosθ

逆 Park（电压方向）：      Vα =  Vd·cosθ - Vq·sinθ
                           Vβ =  Vd·sinθ + Vq·cosθ
```

### 电流环的作用

电流环是 FOC 的内环，也是**最核心的一环**。它的任务是：

```
让实际 Iq = 你想要的 Iq（目标电流）
让实际 Id = 0（不需要直轴磁场）
```

**为什么 Iq 需要闭环？**

Iq 决定了电机的转矩（力矩）：

```
转矩 = Iq × 磁链（常数）
Iq 稳了 → 转矩就稳了 → 电机不抖不震
```

**电流环怎么工作？**

```
目标 Iq ──→ 误差 ──→ PI 控制器 ──→ 电压 ──→ 电机 ──→ 实际 Iq
            ↑                                        │
            └────────────────────────────────────────┘
                        反馈实际 Iq
```

PI 做的事：
- P（比例）：误差大 → 加大电压，让电流快速追上去
- I（积分）：一直有静差 → 积分慢慢累计，直到误差消失

**为什么 Id 设 0？**

Id 产生直轴磁场。永磁体已经有磁场了，再加 Id 只会：
- 浪费电能
- 严重时可能消磁
- 表贴式电机（SPMSM）加了 Id 也不产生转矩

所以 `Id_ref = 0` 是 FOC 的标准做法，让电流环自己去把 Id 压到 0。

**恒流 = 恒转矩**

```
Iq 不变 → 转矩不变 → 恒转矩控制
```

适合电梯、传送带等需要恒定力矩的场景。如果需要恒速，则在电流环外面再加一个速度环。

### Id 与 Iq 的物理意义

```
把电机想象成一头驴：
  Iq = 驴往前走的驱动力（越大越快）
  Id = 驴向上跳的力（我们不需要，让它=0）
```

- **Iq（交轴电流）**：产生电磁转矩，与转矩成正比
- **Id（直轴电流）**：只产生磁场（永磁体已有磁场了），所以 Id = 0

**FOC 控制目标：Id → 0，Iq → 目标值**

### 整体流程小结

```
三相电流 ──Clark──→ αβ（交变） ──Park──→ dq（直流） → PI 控制
```

把交流控制问题简化成直流闭环控制。

---

## 3. FOC 完整控制架构

### 控制框图

```
                    ┌──────┐
  目标速度 ────────→│ 速度PI│ ← 实际速度
                    │  环  │
                    └──┬───┘
                       │ Iq_ref
          Id_ref=0    ┌─┴──┐
          ──────────→│电流 │← Id, Iq
                     │ PI  │
                     │ 环  │
                     └──┬──┘
                        │ Vd, Vq
                     ┌──┴───┐
                     │逆Park│
                     └──┬───┘
                        │ Vα, Vβ
                     ┌──┴───┐
                     │SVPWM │
                     └──┬───┘
                        │ 6路PWM
                     ┌──┴───┐
                     │ 三相桥 │
                     └──┬───┘
                        │
                     ┌──┴───┐
                     │ 电机  │
                     └──┬───┘
                        │ Ia, Ib, Ic
                     ┌──┴───┐
                     │Clarke│
                     └──┬───┘
                        │ Iα, Iβ
                     ┌──┴───┐
                     │ Park │ ← 转子角度 θ
                     └──┬───┘
                        │ Id, Iq
```

### 执行流程（8 步）

| 步骤 | 做什么 | 通俗理解 |
|------|--------|---------|
| ① | 采样三相电流 Ia, Ib, Ic | 知道现在的情况 |
| ② | Clark 变换 → Iα, Iβ | 去掉一个冗余维度 |
| ③ | Park 变换 → Id, Iq | 站到转子上去看 |
| ④ | PI 控制器：Id→0, Iq→目标值 → Vd, Vq | 控制扭矩 |
| ⑤ | 获取转子角度 θ（有感直读/无感估算） | 知道转子在哪 |
| ⑥ | 逆 Park → Vα, Vβ | 跳回地面 |
| ⑦ | SVPWM → 三相电压 → PWM 占空比 | 算出 MOS 怎么开关 |
| ⑧ | 更新 PWM 到三相桥 | 电机转动 |

### 为什么要做这些变换？

三相互差 120°，直接控制三个正弦波很麻烦。想要的效果是**给一个速度就按这个速度转**，而不是去调三个正弦波。坐标变换把三相交流电降维解耦，变成简单的直流控制量（Iq）。

---

## 4. PI 控制器

P（比例）：误差 × Kp，立即响应。只用 P 会有静差。
I（积分）：误差累积 × Ki，消除静差。

**P + I 组合：** P 快速响应，I 消除残余误差。

**整定口诀：**
```
Kp 太大 → 震荡、超调
Kp 太小 → 响应慢

Ki 太大 → 积分饱和、过冲
Ki 太小 → 静差消不掉

先调 P 让系统动起来，再加 I 消除静差
```

### 积分饱和（Integral Windup）

PI 输出达到限幅后，积分器还在继续累加，导致退出饱和慢、超调大：

```
PI 输出 = Kp × 误差 + Ki × ∫误差

输出达到限幅（比如 12V）被"切"住，
但积分器不知道，还在累加，
等需要反向调节时，积分器要先"吐"完才能响应。
```

**比喻：** 踩油门加速到限速 120 松了，但积分器还在帮你踩，车冲到 130 才慢下来。

### 两种抗积分饱和方式

| 方式 | 核心思路 | 适用场景 |
|------|---------|---------|
| **钳位 Clamping** | 输出饱和时冻结积分，停止累加 | V/F 开环、低成本驱动 |
| **反算 Back-Calculation** | 用饱和差值反向修正积分，主动拉回 | FOC 矢量控制、高精度伺服 |

**钳位 Clamping（代码）：**

```c
output = Kp * err + integrator;

// 限幅
if (output > max) output = max;
else if (output < -max) output = -max;

// 抗积分饱和：输出被限幅 → 冻结积分器
if (output >= max || output <= -max) {
    /* 不更新积分器 */
} else {
    integrator += Ki * err * Ts;
}
```

优点：简单，一个 if 搞定
缺点：退出饱和慢

**反算 Back-Calculation（代码）：**

```c
output = Kp * err + integrator;
float before_sat = output;

if (output > max) output = max;
else if (output < -max) output = -max;

// 饱和差值反向修正积分
float diff = output - before_sat;
integrator += Ki * err * Ts + Kc * diff;
```

优点：退出饱和快、超调小
缺点：多一个 Kc 参数要调

**你的项目用哪种？**

```
V/F 开环 → 不需要抗饱和（输出不会饱和）
FOC 电流环 → 钳位（简单可靠）
高精度伺服 → 反算
```

---

## 5. SVPWM（空间矢量 PWM）

### 为什么用 SVPWM？

逆 Park 输出 Vα、Vβ 后，有两种驱动方式：

| | SPWM | SVPWM |
|--|------|-------|
| 直流电压利用率 | 0.785 | **0.906**（高 15.4%） |
| 谐波含量 | 较大 | 小 |
| 复杂度 | 简单 | 较复杂 |

SVPWM 效果更好，也可以用零序注入等效实现。

### SVPWM 与六步换向的关系

6 个 MOS 组成的三个桥臂有 **8 种导通状态**：

- **6 个有效矢量**：一上两下或两上一下（和六步换向一样）
- **2 个零矢量**：全上或全下（六步换向没有这两个）

零矢量就是**零序分量**。通过基础矢量 + 零矢量可以合成任意方向和大小的电压。

### 基本电压矢量表

| 矢量 | ABC 状态 | Ua | Ub | Uc | Uout |
|------|---------|----|----|----|------|
| Uz(000) | 000 | 0 | 0 | 0 | 0 |
| U0(100) | 100 | 2/3Udc | -1/3Udc | -1/3Udc | 2/3Udc |
| U60(110) | 110 | 1/3Udc | 1/3Udc | -2/3Udc | 2/3Udc·e^(jπ/3) |
| U120(010) | 010 | -1/3Udc | 2/3Udc | -1/3Udc | 2/3Udc·e^(j2π/3) |
| U180(011) | 011 | -2/3Udc | 1/3Udc | 1/3Udc | 2/3Udc·e^(jπ) |
| U240(001) | 001 | -1/3Udc | -1/3Udc | 2/3Udc | 2/3Udc·e^(j4π/3) |
| U300(101) | 101 | 1/3Udc | -2/3Udc | 1/3Udc | 2/3Udc·e^(j5π/3) |
| Uz(111) | 111 | 0 | 0 | 0 | 0 |

有效矢量幅值均为 **2/3 Udc**，相邻夹角 60°，构成正六边形顶点。

### 扇区划分

```
               U120(010)          U60(110)
                   ↑           ↗
                    \   Ⅱ  Ⅰ  /
                     \       /
            U180(011)←————→ U0(100)
                     /       \
                    /  Ⅲ  Ⅵ  \
                   ↓           ↘
              U240(001)          U300(101)
```

6 个扇区（Ⅰ~Ⅵ），目标电压 Uref 落在其中一个扇区内。

**合成规则（伏秒平衡）：**

$$
U_{ref} \cdot T_s = U_4 \cdot T_4 + U_6 \cdot T_6 + U_0 \cdot T_0
$$

在一个 PWM 周期内，用相邻两个有效矢量 + 零矢量合成目标电压。

### 零序电压注入（马鞍波调制）

**为什么要注入？** SPWM 正弦波峰值高，容易顶到电源电压天花板，削波失真。

**思路：** 在正弦波上叠一个共模电压，把波峰往下压、波谷往上抬，电压利用率更高。

**三相调制波公式：**
$$
\begin{cases}
V_{am} = V_m \sin\omega t + V_0 \\
V_{bm} = V_m \sin(\omega t - 2\pi/3) + V_0 \\
V_{cm} = V_m \sin(\omega t + 2\pi/3) + V_0
\end{cases}
$$

零序分量：
$$
V_0 = -\dfrac12(V_{max} + V_{min})
$$


```
原始正弦波：~~~~~~~~~
               +
马鞍波：    ┌┐    ┌┐     ← 顶部削平，能输出更高电压
```

直流电压利用率从 **0.785 → 0.906**（提升 15.4%）。

**为什么 SVPWM 比 SPWM 利用率高？**

#### 先看硬件限制

三相逆变器每相只能输出 Vdc 或 GND（MOS 管只有开关两种状态）。

```
              VDC
               │
     ┌── 上桥 ─┴── 上桥 ──┴── 上桥
     │         │         │
     U         V         W
     │         │         │
     └── 下桥 ─┴── 下桥 ──┴── 下桥
               │
              GND
```

#### SPWM 的限制

SPWM 每相单独用正弦波和三角波比较。先看每相的硬件结构（半桥）：

```
VDC ────┤ 上桥 ├────┬──── 输出到电机
         └──────┘    │
         ┌──────┐    │
GND ────┤ 下桥 ├────┘
         └──────┘
```

上桥导通 → 输出 = VDC，下桥导通 → 输出 = GND。**只有这两种状态，没有中间电压。**

那怎么得到正弦波？靠 PWM 占空比来**平均**出中间电压：

```
占空比 50%:  ████░░░░    平均电压 = VDC × 50% = Vdc/2   ← 中点
占空比 75%:  ██████░░    平均电压 = VDC × 75%
占空比 25%:  ██░░░░░░    平均电压 = VDC × 25%
占空比 0%:   ░░░░░░░░    平均电压 = 0V（GND）
占空比 100%: ████████    平均电压 = VDC
```

SPWM 的正弦波和三角波比较，正弦波中点在 Vdc/2（50% 占空比），上下对称摆动：

```
  相电压波形
  Vdc ──┐
        │  ~~~~~~~~
  Vdc/2─┤~~~~~~~~~~      ← 中点在 Vdc/2（对应 50% 占空比）
        │  ~~~~~~~~
  GND ──┘
```

正弦波峰顶要 < VDC，峰谷要 > GND：

```
峰顶 = Vdc/2 + A  ≤  VDC     →  A ≤ Vdc/2
峰谷 = Vdc/2 - A  ≥  GND(0)  →  A ≤ Vdc/2
```

所以振幅 A 最大 = Vdc/2，**SPWM 相电压峰值 = Vdc/2。**

#### SVPWM 如何突破

SVPWM 不走单相正弦波路线，而是用电压矢量合成。6 个有效矢量的幅值都是 **2/3 × Vdc**，围成一个**正六边形**：

```
              U120(010)
                  ↑
                  │  2/3·Vdc
        U180 ←────┼────→ U0(100)
                  │
                  ↓
              U240(001)
```

SVPWM 可以合成六边形内的任意矢量。最大能输出的圆形轨道是六边形的**内切圆**：

```
              ┌──── 顶点：2/3·Vdc
              │
        ┌─────┼─────┐
        │    ╱      │
        │   ╱内切圆  │
        │  ╱ 半径    │
        │   Vdc/√3  │ ← 最大相电压
        └───────────┘
```

从六边形中心到边的垂直距离（内切圆半径）：

```
(2/3·Vdc) × cos30° = (2/3·Vdc) × (√3/2) = Vdc/√3
```

所以 **SVPWM 相电压峰值 = Vdc/√3。**

#### 直接对比

```
              相电压峰值      对比
SPWM:         Vdc/2  = 0.500  基准
SVPWM:        Vdc/√3 = 0.577  高 15.4%
```

提升比例 = (Vdc/√3) / (Vdc/2) = 2/√3 ≈ 1.154

若以六步法（方波驱动）的理论最大线电压有效值 0.780Vdc 为基准：

```
              线电压有效值    对比六步法
SPWM:          0.612Vdc        0.785
SVPWM:         0.707Vdc        0.906
六步法基准:     0.780Vdc        1.000
```

SPWM 达到六步法的 78.5%（0.785），SVPWM 达到 90.6%（0.906）。

#### 一句话

**SPWM 受限于"每相单独看都是正弦波"，波峰到顶就削波。SVPWM 跳出单相正弦波思路，从三相整体用矢量合成，相电压多榨了 15.4%。**

### CCR 计算步骤（OC 调制）

这是实际代码里算 CCR 的流程：

**① 逆 Clark 得到三相正弦电压：** PI 输出 Vd、Vq → 逆 Park → Vα、Vβ → 逆 Clark → Va、Vb、Vc。

**② 加零序分量变成马鞍波：** V0 = -0.5 × (Vmax + Vmin)，然后加到三相上。

**③ 乘 -1 匹配 PWM 模式：** TIM1 配了模式 1（CNT > CCR 输出低电平），乘 -1 翻转极性，匹配硬件逻辑。

**④ 除以 Vbus 得到占空比比例：** duty_raw = Vc_m / Vbus。20V 母线、算出来 10V → 0.5 → 50%。

**⑤ 加 0.5 将负值抬升到正：**

```
duty = duty_raw + 0.5
```

| Vc | duty_raw | +0.5 | 占空比 | 说明 |
|----|----------|------|--------|------|
| +Vdc/√3 | 0.577 | 1.077 | 100%（限幅） | 过调制区 |
| 0 | 0 | 0.5 | 50% | 中点 |
| -Vdc/√3 | -0.577 | -0.077 | 0%（限幅） | 过调制区 |

0.5 就是 Vdc/2 与 Vbus 的比值（50% = 中点），硬件决定的。

**⑥ 乘 ARR 得到 CCR：**

```
CCR = (Vc_m / Vbus + 0.5) × 8000
```

SVPWM 峰值 Vc = Vdc/√3 时 duty 超过 1，所以要限幅：

```c
if (duty > 1.0) duty = 1.0;
if (duty < 0.0) duty = 0.0;
```

这就是马鞍波的形成——顶部被削平。

### SVPWM 逻辑总结

```
① 硬件基础：6 个有效矢量 + 2 个零矢量 → 六边形电压边界
② 矢量合成：目标电压 → 对应扇区 → 伏秒平衡分配导通时间
③ 软件实现：零序电压注入 → 马鞍形调制波 → 输出圆形旋转磁场
```

---

## 6. 电流采样与 ADC 配置

### 采样电路参数

| 参数 | 值 |
|------|-----|
| 采样电阻 | 5mΩ（R32/R33/R34） |
| 运放增益 | 11kΩ / 1.5kΩ ≈ 7.33 倍 |
| 共模偏置 | 1.65V（22kΩ 上拉到 3.3V） |
| MOS 管 | BSC0702LS，栅极电阻 33Ω，自举电容 100nF |

### 电流计算公式

ADC 数字量 x（12bit, 0~4095），减去偏置再算：

$$
I = (x - 2048) \times 0.021972
$$

> 2048 对应 1.65V 偏置，x > 2048 为正电流，x < 2048 为负电流。

系数 0.021972 的推导：

$$
I = \frac{x \times 3.3}{4096 \times 0.005 \times 7.33} \approx x \times 0.021972
$$

### 母线电压检测

分压：VDC → R45(75kΩ) → PA0 → R46(3kΩ) → GND

$$
V_{DC} = x \times 0.020947
$$

$$
\frac{V_{DC}}{75k+3k} = \frac{V_{ADC}}{3k}, \quad V_{ADC} = \frac{x}{4096} \times 3.3
$$

### 注入组模式

FOC 需要在 PWM 周期中点同步采样，因此三个采样通道都配为**注入组**，由 TIM1_CH4 硬件触发：

```
PWM 周期（中心对齐）：
CNT: 0 ──→ 中点 ←──→ 中点 ←── 0
                   ↑
             TIM1_CH4 触发 ADC
             避开 MOS 开关噪声，电流最稳定
```

### 实际代码实现（项目代码）

#### 全局变量

```c
uint8_t ADC_OffSet = 0;          // 偏置校准完成标志
static uint8_t cnt = 0;          // 校准计数器
static float offset[3] = {0};    // 三相偏置值
static uint32_t ADC_Data[3] = {0};
float CurrlValue[3] = {0};       // 最终电流值
float raw0, raw1, raw2;          // 原始值临时变量
static float last_val[3] = {0};  // 上次值（滤波用）
```

#### BSP_ADC_Init

```c
void BSP_ADC_Init(void)
{
    HAL_OPAMP_Start(&hopamp1);
    HAL_OPAMP_Start(&hopamp2);
    HAL_OPAMP_Start(&hopamp3);

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);

    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_JEOC);
    __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_JEOC);

    HAL_ADCEx_InjectedStart_IT(&hadc1);
    HAL_ADCEx_InjectedStart(&hadc2);
}
```

#### 注入转换回调

```c
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC1)
    {
        if(ADC_OffSet == 0)          // 校准阶段
        {
            cnt++;
            offset[0] += ADC1->JDR1;
            offset[1] += ADC2->JDR1;
            offset[2] += ADC1->JDR2;
            if(cnt >= 10)
            {
                offset[0] /= 10.0f;
                offset[1] /= 10.0f;
                offset[2] /= 10.0f;
                ADC_OffSet = 1;
            }
        }
        else                         // 正常运行
        {
            ADC_Data[0] = ADC1->JDR1;
            ADC_Data[1] = ADC2->JDR1;
            ADC_Data[2] = ADC1->JDR2;

            raw0 = (ADC_Data[0] - offset[0]) * 0.021972f;
            raw1 = (ADC_Data[1] - offset[1]) * 0.021972f;
            raw2 = (ADC_Data[2] - offset[2]) * 0.021972f;

            // 一阶低通滤波 alpha=0.3
            CurrlValue[0] = last_val[0] + 0.3f * (raw0 - last_val[0]);
            CurrlValue[1] = last_val[1] + 0.3f * (raw1 - last_val[1]);
            CurrlValue[2] = last_val[2] + 0.3f * (raw2 - last_val[2]);

            last_val[0] = CurrlValue[0];
            last_val[1] = CurrlValue[1];
            last_val[2] = CurrlValue[2];
        }
    }
}
```

#### 母线电压读取

```c
float get_bus_voltage(void)
{
    float sum = 0;
    for(int i = 0; i < 10; i++) {
        ADC1->CR |= ADC_CR_ADSTART;
        while (!(ADC1->ISR & ADC_ISR_EOC));
        uint32_t val = ADC1->DR;
        sum += val * 0.020947f;
    }
    return sum / 10.0f;
}
```

#### main 循环发送到 VOFA+

```c
while (1)
{
    voltage = get_bus_voltage();

    if (ADC_OffSet == 1)
    {
        static uint8_t vofaData[20];
        memcpy(vofaData,       CurrlValue, 4);        // U 相
        memcpy(vofaData + 4,   CurrlValue + 1, 4);    // V 相
        memcpy(vofaData + 8,   CurrlValue + 2, 4);    // W 相
        memcpy(vofaData + 12,  &voltage, 4);           // 母线电压

        vofaData[16] = 0x00; vofaData[17] = 0x00;
        vofaData[18] = 0x80; vofaData[19] = 0x7F;

        HAL_UART_Transmit_DMA(&huart3, vofaData, 20);
        HAL_Delay(1);
    }
}
```

---

## 7. 转子角度估算

### 有感方案一：编码器

$$
\theta = (N / PPR) \times 360^\circ
$$

N 为脉冲计数，PPR 为每圈脉冲数。1024 线编码器分辨率约 0.35°。

### 有感方案二：霍尔传感器

霍尔只有 3 个 bit，6 种状态，每 60° 变一次：

```
霍尔值： 101 → 001 → 011 → 010 → 110 → 100 → 101
```

### 核心原理：角度属于「跳变沿」，不属于「霍尔状态」

电角度是霍尔跳变边沿的固有物理属性，和电机转向无关。
每个霍尔状态有两条边沿（入口沿和出口沿），正反转进入同一个状态时跨过的是不同的边沿，所以角度不同。

**用环形公路的里程碑来理解：**

```
电气圆周上一共 6 条霍尔跳变沿 = 6 块里程碑
每块牌子上刻着固定的公里数（电角度）
正转 = 顺时针开车，反转 = 逆时针开车
无论哪个方向，路过同一块里程碑时，牌子上的数字永远不变
```

### 6 条边沿的固定电角度

本项目霍尔正转顺序：**1 → 5 → 4 → 6 → 2 → 3 → 1**
反转顺序：**1 → 3 → 2 → 6 → 4 → 5 → 1**

6 条跳变沿及其固定的电角度（假设 PHASE_SHIFT = 0）：

| 边沿（状态跳转） | 固定电角度 | 代码中的常量 |
|:---:|:---:|:---:|
| 1↔5 | 0° | `PHASE_SHIFT_ANGLE` |
| 5↔4 | 60° | `PHASE_SHIFT_ANGLE + PI/3` |
| 4↔6 | 120° | `PHASE_SHIFT_ANGLE + 2*PI/3` |
| 6↔2 | 180° | `PHASE_SHIFT_ANGLE + PI` |
| 2↔3 | 240° | `PHASE_SHIFT_ANGLE + 4*PI/3` |
| 3↔1 | 300° | `PHASE_SHIFT_ANGLE + 5*PI/3` |

### 代码逻辑验证

以状态 4 为例（用户代码的注释）：

```
case 4:
    if (Prev == 5)  // 正转 5→4：跨过边沿「5↔4」
        MeasuredElAngle = PHASE_SHIFT + PI/3    ← 60°，正转
    if (Prev == 6)  // 反转 6→4：跨过边沿「4↔6」
        MeasuredElAngle = PHASE_SHIFT + 2*PI/3  ← 120°，反转
```

**正反转跨过同一条边沿 → 角度相同：**

```
边沿「4↔6」固定角度 = 120°（2PI/3）

正转 4→6 跨过它 → 进入状态 6 → 角度 = 2PI/3
反转 6→4 跨过它 → 进入状态 4 → 角度 = 2PI/3（相同！）
```

**正反转进入同一个状态 → 角度不同：**

```
正转进入状态 4：跨过「5↔4」边沿 → 角度 = PI/3（60°）
反转进入状态 4：跨过「4↔6」边沿 → 角度 = 2PI/3（120°）
同一个状态 4，两条不同边沿，角度差 60°
```

**总结表：**

| 场景 | 角度是否相同 | 原因 |
|------|------------|------|
| 正反转跨过**同一条边沿** | ✅ 完全相同 | 同一条物理边界，角度固定 |
| 正反转进入**同一个霍尔状态** | ❌ 差 PI/3（60°） | 从两个不同的边沿进入 |

### 边沿对齐 vs 中点对齐

两种主流标定方式：

| 方式 | 含义 | 何时使用 |
|------|------|---------|
| **边沿对齐** | 霍尔跳变沿 = 区间起点，不加 PI/6 | 中断中（转子转动时） |
| **中点对齐** | 跳变沿是边界，磁链中心在区间正中，加 PI/6 | Init（上电静止时） |

**为什么 Init 用中点对齐？** 上电时转子停在扇区任意位置，不知道方向，取区间中点作为初始角度，误差最小。

**为什么中断用边沿对齐？** 霍尔跳变时确切知道转子到了边界，跳变沿就是基准起点，不需要加 PI/6。同时靠前一状态判断正反转，给不同基础角度。

**PHASE_SHIFT 是安装偏差（每台电机不同），PI/6 是区间中点与边沿的固有差值（固定 30°），两者是两套独立的补偿，不要混淆。**

**不能混用的关键点：**
```
Init 的 +PI/6 拿到中断里用 → 角度偏 30°，力矩波动
中断的不加 PI/6 拿到 Init 里用 → 角度偏 30°，启动抖动
```

霍尔每 60° 才变一次，中间角度用速度推算：

$$
\theta(k) = \theta_{跳变} + \omega \cdot \Delta t
$$

起步用霍尔（±5°），高速时霍尔只做校正。

### 霍尔驱动代码实现（实战）

硬件连接：PB6/PB7/PB8 → TIM4_CH1/CH2/CH3（霍尔模式）

**① 结构体定义（Hall_Driver.h）：**
```c
typedef struct {
    uint8_t HallState;        // 当前霍尔值 (3bit)
    uint8_t PrevHallState;    // 上次霍尔值
    float HallElAngle;        // 最终电角度 (0~2π)
    float MeasuredElAngle;    // 霍尔跳变时的精确角度
    float ElSpeed;            // 电角速度 (rad/s)
    float SpeedRPM;           // 机械转速
    int8_t Direction;         // 1=正转, -1=反转
    uint32_t CapturedValue;   // TIM4 捕获值
} HALL_Handle_t;
```

**② 读霍尔状态（HALL_GetState）：**
```c
uint8_t HALL_GetState(void)
{
    uint8_t state = 0;
    state  = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);       // bit0
    state |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) << 1;  // bit1
    state |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) << 2;  // bit2
    return state;
}
```

**③ 初始角度（HALL_InitAngle）：**
上电读一次霍尔，确定当前 60° 扇区。扇区编号和角度映射：

| 状态 (C-B-A) | 电角度 |
|:---:|:---:|
| 5 (101) | 0 |
| 1 (001) | π/3 |
| 3 (011) | 2π/3 |
| 2 (010) | π |
| 6 (110) | 4π/3 |
| 4 (100) | 5π/3 |

这个映射取决于霍尔安装方式，不对就调换 case 顺序。

**④ 跳变更新（HALL_UpdateAngle）：**
在 TIM4 中断回调里调，每 60° 一次：

- 读 TIM4 捕获值 → 算电角速度：ElSpeed = (π/3) / (CCR / TIM_CLK)
- 比较新旧状态 → 判断正反转
- 用新状态对应的精确角度重置 HallElAngle（消除累积误差）

**⑤ 霍尔测速公式（转速 RPM）：**

霍尔 60° 跳变一次，TIM4 捕获两次跳变之间的计数值，通过这个值算转速：

```c
// TIM4 时钟 3.2MHz（160MHz ÷ 50 分频）
// capture 是两次霍尔跳变之间的计数值

// ① 算电角速度（rad/s）
//    60° = π/3 rad，用时 = capture / 3.2M（秒）
//    电角速度 = (π/3) / (capture / 3.2M)

// ② 转 RPM
//    RPM = rad/s × 60 / (2π) = 角速度 × 30 / π
//    30/π ≈ 9.55，是 rad/s 转 RPM 的固定系数

TempSpeed = (PI/3) / (capture / 3200000) * 30 / PI;
```

如果不想记系数，可以按物理意义理解：

```
RPM = （60° 内转了多少圈）× 60 ÷ 极对数
    = 频率（Hz）× 60 ÷ 极对数
```

**⑥ ADC 回调中的插值：**
```c
// 两次霍尔跳变之间（60° 内），用速度累加插值
HALL_Handle.HallElAngle += HALL_Handle.ElSpeed * Ts;
```

### 霍尔角度注入 FOC 模型

MATLAB 生成的 `FOC_Model` 内部有 SMO 观测器，但可以通过 `rtU.theta` 接口用外部角度覆盖：

```c
rtU.theta = HALL_Handle.HallElAngle;   // 霍尔真实角度
rtU.theta_enable = 1.0f;              // 启用外部角度
FOC_Model_step();                      // 模型内部用外部角度做 Park/逆Park
```

在 `ExtU` 结构体中添加：
```c
typedef struct {
    ...                  // 原有字段
    real32_T theta;      // 外部输入角度
    real32_T theta_enable; // =1 用外部角度，=0 用内部观测器
} ExtU;
```

`FOC_Model_step` 中所有给 `rtDW.Merge`（角度）赋值的地方改为：
```c
rtDW.Merge = (rtU.theta_enable > 0.5F) ? rtU.theta : [内部角度];
```

### 无感方案：滑模观测器（SMO）— 本项目使用

电机电压方程在 αβ 坐标系下：

$$
\begin{aligned}
V_\alpha &= R I_\alpha + L \frac{dI_\alpha}{dt} + E_\alpha \\
V_\beta &= R I_\beta + L \frac{dI_\beta}{dt} + E_\beta
\end{aligned}
$$

移项得到反电动势：

$$
\begin{aligned}
E_\alpha &= V_\alpha - R I_\alpha - L \frac{dI_\alpha}{dt} \\
E_\beta &= V_\beta - R I_\beta - L \frac{dI_\beta}{dt}
\end{aligned}
$$

| 符号 | 怎么来 |
|------|--------|
| Vα, Vβ | PWM 占空比 + 母线电压算出 |
| Iα, Iβ | 采样电流 → Clarke 变换 |
| dI/dt | 相邻采样差分近似 |
| R, L | 电机参数（测量或辨识） |

反电动势包含角度：

$$
\begin{aligned}
E_\alpha &= -\omega \psi_f \sin\theta \\
E_\beta &= \omega \psi_f \cos\theta
\end{aligned}
$$

两式相除消去 $\omega\psi_f$：

$$
\frac{E_\alpha}{E_\beta} = -\tan\theta \quad\Rightarrow\quad \boxed{\theta = \operatorname{atan2}(-E_\alpha, E_\beta)}
$$

### atan2 详解

| | atan(y/x) | atan2(y, x) |
|--|-----------|-------------|
| 输出范围 | -90°~+90° | **-180°~+180°** |
| 象限判断 | 会搞错 | 自动识别 |

**公式含义：** 把反电动势矢量 (Eα, Eβ) 转成新矢量 (Eβ, -Eα)，atan2 算这个矢量与 x 轴的夹角，就是转子电角度。

**C 代码：**

```c
float theta = atan2f(-Ealpha, Ebeta);
```

**SMO 核心流程：**

```
Iα, Iβ（测量） ────→ ┌──────────┐ ───→ Eα, Eβ
Vα, Vβ（计算） ────→ │ SMO 观测器 │
                      └──────────┘
                           │
                           ↓
                     θ = atan2(-Eα, Eβ)
                     ω = dθ/dt
```

通俗理解：① 预测电流 → ② 和实测比较 → ③ 修正反电动势 → 收敛到正确角度。每个 PWM 周期跑一次。

---

## 8. 无感 FOC 启动策略

无感 FOC 的先天问题：**转速=0 → 反电动势=0 → 观测器估不出角度**。

启动需要三个阶段：

```
阶段一：定位（开环）
  给一个固定的 Id，转子被迫对齐到某个角度

阶段二：I/F 强拖（开环）
  慢慢加大旋转频率，拖着转子转起来
  转速上升到 5%~10% 额定转速

阶段三：切闭环
  反电动势足够大 → 观测器能工作了
  平滑切换到闭环 FOC
```

---

## 9. G431 为什么适合 FOC？

| 特性 | 作用 |
|------|------|
| HRTIM | 产生 SVPWM，硬件死区插入 |
| 3×PGA | 电流采样放大，省掉外部运放 |
| 12bit ADC | 同步采样两相电流 |
| 170MHz M4 | 电流环 10~20kHz 无压力 |
| CORDIC 加速 | 硬件 sin/cos（Park 变换需要） |

---

## 10. CAN / FDCAN 通信

### CAN 是什么？

CAN 是一种串口通信，和 USART 的区别：USART 是点对点（两个人打电话），CAN 是多机互联（一群人用对讲机）。

```
USART：A ──→ B（最多两台设备）
CAN：  A ←──→ B ←──→ C（所有设备挂两根线上）
```

### 为什么电机控制用 CAN？

| | USART 串口 | CAN |
|--|-----------|-----|
| 连线 | TX、RX、GND 三根线 | CAN_H、CAN_L **两根线** |
| 距离 | 几米 | 几十米到上千米 |
| 抗干扰 | 差（电平信号） | **好**（差分信号） |
| 多机 | 点对点最多 2 个 | **上百个设备** |
| 实时性 | 不好 | **好**（ID 越小优先级越高） |

### 物理接口与电平

CAN 总线用两根线（CAN_H、CAN_L）传输，通过两根线的**电位差**判断逻辑电平（IOS11898 标准）：

```
逻辑 0（显性电平）：   CAN_H = 3.5V,  CAN_L = 1.5V   → 电位差 = 2V
逻辑 1（隐性电平）：   CAN_H = 2.0V,  CAN_L = 2.0V   → 电位差 = 0V
```

显性电平会"覆盖"隐性电平——只要有一个设备拉出显性，总线上就是显性。这就是仲裁的基础：发 0 的会覆盖发 1 的。

### 差分信号（抗干扰的关键）

CAN 两根线拧在一起传，外部干扰同时作用到两根线上，电压差不变，所以抗干扰强。

```
串口：测单线对 GND 电压 → 干扰就错
CAN： 测两根线电压差   → 干扰同时影响两根线 → 差不变
```

### 为什么需要 120Ω 终端电阻？

CAN 总线两端各并联一个 **120Ω** 电阻，作用：**消除信号反射**。

```
设备1 ────┐                ┌──── 设备2
          │                │
      ┌──┴──┐          ┌──┴──┐
      │120Ω │   CAN_H  │120Ω │
      │     ├──────────┤     │    ← 两端各一个 120Ω
      │120Ω │   CAN_L  │120Ω │
      └──┬──┘          └──┬──┘
          │                │
          └────────────────┘
```

不加终端电阻的问题：信号传到总线末端会**反射回来**，和后面的信号叠加，造成波形畸变、误码。

```
没有终端电阻：          有终端电阻：
┌─────────┐             ┌─────────┐
│ 信号   │              │ 信号    │
│       ╲│╱ 反射        │        │   ← 反射被吸收，波形干净
│        │              │         │
└─────────┘             └─────────┘
```

注意：**120Ω 只加在总线两端**（最远的两个节点），中间节点不加。你的板子端子并联的 R8（120Ω）就是做这个用的。

### CAN 仲裁（抢话筒）

每个设备发数据前先发一个 ID（数字越小优先级越高），多个设备同时发时自动比较，ID 最小的继续发，其他的退让：

```
ID=0x001（电机驱动器，赢）
ID=0x002（编码器，输）
ID=0x003（显示器，输）
```

### FDCAN vs 标准 CAN

FDCAN 是 CAN 的升级版，STM32G431 支持 FDCAN。

| 对比项 | 标准 CAN | FDCAN |
|--------|---------|-------|
| 最高速率 | 1Mbps | **8Mbps**（数据段） |
| 一帧数据长度 | **8 字节** | **64 字节** |
| 帧格式 | 标准帧/扩展帧 | 新增 FDF、BRS、ESI 位 |
| 兼容性 | — | 向下兼容标准 CAN |

**数据长度的意义：** 你要发三相电流 + 母线电压 + 转速 + 温度，标准 CAN 要拆好几帧，FDCAN 一帧 64 字节足够了。

### 帧类型

- **数据帧**：传数据，最常用
- **遥控帧**：请求其他节点发数据
- **错误帧**：检测到总线错误时通知所有节点
- **过载帧**：接收节点来不及处理时请求延迟

### FDF、BRS、ESI 的含义

这三个位是 FDCAN 帧头里的标志位，告诉接收方怎么处理这帧：

| 位 | 全称 | 含义 |
|----|------|------|
| **FDF** | FD Format | =0 标准 CAN 帧（8 字节，不加速）；=1 FDCAN 帧（64 字节，可加速） |
| **BRS** | Bit Rate Switch | =0 全程不加速；=1 仲裁段 1Mbps，数据段切到高速，发完回到 1Mbps |
| **ESI** | Error State Indicator | =0 发送节点正常；=1 发送节点已累积较多错误（被动错误状态） |

**一句话：** FDF 告诉你是新格式，BRS 告诉你要加速，ESI 告诉你发这帧的设备有没有出问题。

### 分两段跑（FDCAN 加速原理）

```
仲裁段（抢话筒）：1Mbps（所有设备统一）
数据段（传数据）：5~8Mbps（加速）
```

抢话筒时低速公平，传数据时高速高效。

### 项目中的 CAN 硬件

```
PCANVIEW（PC 上位机）
  │
  │ CAN_H / CAN_L（两根线，120Ω 终端电阻）
  │
  └── L9616 CAN 收发器
         │
         │ PA13(TX) / PA14(RX)
         │
     STM32G431（你的板子）
```

- 芯片：**L9616** CAN 驱动芯片
- 引脚：PA13（FDCAN_TX）、PA14（FDCAN_RX）
- 用途：PCANVIEW 上位机发送调速指令
- 终端电阻：差分端并联 **120Ω**（R8）

### CubeMX 配置步骤

#### 1. 基本功能参数

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| Clock Divider | 1 | 不分频，直接用内核时钟 |
| Frame Format | Classic CAN | 经典 CAN 模式，不启用 CAN-FD |
| Mode | Normal Mode | 正常通信。其他：Loopback（自环回测）、Silent（只收不发） |
| Auto Retransmission | Disable | 禁止自动重传 |
| Transmit Pause | Disable | 禁止传输暂停 |
| Protocol Exception | Disable | 禁止协议异常处理 |

#### 2. 波特率参数

公式：波特率 = 时钟频率 / [Prescaler × (1 + TimeSeg1 + TimeSeg2)]

| 参数 | 说明 |
|------|------|
| Nominal Prescaler | 分频器，降低总线时钟 |
| Nominal Sync Jump Width | 同步跳跃宽度 |
| Nominal Time Seg1 | 相位缓冲段 1，控制采样点位置 |
| Nominal Time Seg2 | 相位缓冲段 2，数据校验 |
| Data Prescaler / Seg1 / Seg2 | 同上，但用于 CAN-FD 数据阶段 |

计算示例：时钟 160MHz，Prescaler=20，TimeSeg1=10，TimeSeg2=5

```
波特率 = 160MHz / [20 × (1 + 10 + 5)] = 500kbps
```

#### 3. 过滤器与队列

| 参数 | 说明 |
|------|------|
| Std Filters Nbr | 标准过滤器数量（11 位 ID） |
| Ext Filters Nbr | 扩展过滤器数量（29 位 ID） |
| Tx Fifo Queue Mode | 发送队列，先入先出 |

### CAN ID 的作用

CAN ID 是设备的身份标识。同一网络中每个模块的 ID 一般不同，主机通过 ID 区分消息发给谁。

```
大灯模块：只接收 ID=1~10
雨刷模块：只接收 ID=11~20
主机发 ID=3 → 大灯收到，雨刷忽略
```

在你的项目中，PCANVIEW 上位机通过分配的唯一 CAN ID 向驱动器发送调速指令。

### 代码实现

#### 初始化

```c
void Can_Message_Init(void)
{
    // 滤波器：接收 ID 0x000~0x7FF 的报文
    FDCAN_FilterTypeDef Filter = {0};
    Filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    Filter.FilterID1 = 0x000;
    Filter.FilterID2 = 0x7FF;
    Filter.FilterIndex = 0;
    Filter.FilterType = FDCAN_FILTER_RANGE;
    Filter.IdType = FDCAN_STANDARD_ID;
    HAL_FDCAN_ConfigFilter(&hfdcan1, &Filter);

    // TxHeader 配置（经典 CAN，8 字节，ID=0x125）
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.Identifier = 0x125;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;

    // 使能接收中断（第三个参数必须传 FDCAN_RX_FIFO0，不能传 0）
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_RX_FIFO0);

    // 启动 FDCAN
    HAL_FDCAN_Start(&hfdcan1);
}
```

#### 发送

```c
uint8_t FDCan_TxData[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, FDCan_TxData);
```

#### 接收回调

```c
uint8_t FDCan_RxData[8] = {0};

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        if(hfdcan->Instance == FDCAN1)
        {
            HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, FDCan_RxData);
            // FDCan_RxData[0~7] 即收到的数据，在这里处理
        }
    }
}
```

---

## 11. V/F 控制（简易开环）

### 适用场景

当没有位置传感器、又想让电机转起来时，可以用 V/F 控制。给固定的 Ud、Uq 和频率，电机开环旋转。

```
给 Ud = 0, Uq = 12V, f = 4Hz
  → 产生旋转磁场 → 拖着转子转
  → 不需要知道转子角度
```

### V/F 控制代码实现

#### 手写版 V/F

```c
// 变量
float vf_theta = 0.0f;      // 电角度
float vf_freq = 4.0f;       // 目标频率 Hz
float vf_voltage = 12.0f;   // 目标电压 V

void VF_Step(void)
{
    // 角度累加（10kHz 对应 0.0001s）
    vf_theta += 2 * pi * vf_freq * 0.0001f;
    if (vf_theta > 2*pi) vf_theta -= 2*pi;

    // 逆 Park：Ud=0, Uq=电压
    Ualpha = -vf_voltage * sin(vf_theta);
    Ubeta  =  vf_voltage * cos(vf_theta);

    // 逆 Clark
    Ua = Ualpha;
    Ub = -0.5*Ualpha + 0.866*Ubeta;
    Uc = -0.5*Ualpha - 0.866*Ubeta;

    // 零序注入 → CCR
    V0 = -0.5*(max(Ua,Ub,Uc) + min(Ua,Ub,Uc));
    duty_a = (-(V0+Ua)/Vbus + 0.5) * ARR;
    // ... duty_b, duty_c ...

    // 限幅并设 PWM
}
```

#### Simulink 生成版 V/F（VFmode.c）

用 Simulink 建模生成的 V/F 控制代码，接口如下：

**输入结构体 `ExtU rtU`：**

| 字段 | 类型 | 说明 | 典型值 |
|------|------|------|--------|
| `rtU.ud` | float | 直轴电压（V） | 0 |
| `rtU.uq` | float | 交轴电压（V） | 12 |
| `rtU.Freq` | float | 目标频率（Hz） | 4 |
| `rtU.Vbus` | float | 母线电压（V） | 24 |

**输出结构体 `ExtY rtY`：**

| 字段 | 类型 | 说明 |
|------|------|------|
| `rtY.tABC[0]` | float | U 相 CCR 值 |
| `rtY.tABC[1]` | float | V 相 CCR 值 |
| `rtY.tABC[2]` | float | W 相 CCR 值 |

**使用方式（在 ADC 回调中）：**

```c
#include "VFmode.h"

// 初始化（main 中调一次）
VFmode_initialize();

// ADC 回调中每周期调
rtU.ud = 0.0f;
rtU.uq = 12.0f;
rtU.Freq = 4.0f;
rtU.Vbus = 24.0f;
VFmode_step();

// 写 PWM（注意相序）
TIM1->CCR1 = (uint32_t)rtY.tABC[2];   // CH1(PA8)=W
TIM1->CCR2 = (uint32_t)rtY.tABC[1];   // CH2(PA9)=V
TIM1->CCR3 = (uint32_t)rtY.tABC[0];   // CH3(PA10)=U
```

**VFmode.c 内部流程（5 步）：**

```
① 角度积分：θ += 2π × Freq × Ts
② 角度限幅：0~2π
③ 逆 Park：Vd/Vq → Vα/Vβ（用当前角度 θ）
④ 逆 Clark + 零序注入：Vα/Vβ → Va/Vb/Vc → CCR
⑤ 保存角度（给下一次用）
```

V/F 控制没有电流反馈，速度会随负载变化，适合简单验证电机是否能转。

#### V/F 调参口诀

```
电机不转     → rtU.uq 太小，试试 12V 以上
电机抖动     → rtU.Freq 太低，试试 5Hz 以上
电机嗡嗡响不转 → 相序反了，交换两相试试
转速太慢     → 增大 rtU.Freq
力矩不够     → 增大 rtU.uq（不要超过 Vbus）
```

### SguanFOC 开源库

[SguanFOC](https://gitee.com/star-must-dust-sguan/SguanFOC) 是一个纯 C 语言的开源 FOC 算法库，MIT 协议。

**版本选择：**

| 版本 | 类型 | 适用 |
|------|------|------|
| v3.0.0 | 有感 FOC，浮点运算 | 有霍尔/编码器时使用 |
| v3.1.0 | 无感 FOC，浮点运算 | 无传感器，需 SMO 观测器 |

**核心架构：**

```
SguanFOC_High_Loop()  → 40kHz 电流环（读ADC、算FOC、设PWM）
SguanFOC_Low_Loop()   → 1kHz 状态机（启停、保护、速度环）
SguanFOC_main_Loop()  → 主循环通信（串口协议）
```

**硬件适配（UserData_Function.h）：**
- `User_ReadADC_Raw()` — 读取 ADC 电流采样值
- `User_PwmDuty_Set()` — 设置三相 PWM 占空比
- `User_Encoder_ReadRad()` — 读取编码器角度（有感用）
- `User_VBUS_DataGet()` — 读取母线电压

**注意：** v3.0.0 为有感版本，没有位置传感器时无法运行。如需无感 FOC，换 v3.1.0 或手写 SMO 观测器。

---

## 一句话总结 FOC

> **FOC 通过坐标变换，把"控制三相正弦交流电"变成了"控制两个直流分量 Id 和 Iq"，从而像直流电机一样简单、精确地控制扭矩和速度。**

---

## 附录：有感 FOC 调试完整记录

### 一、问题清单

| 现象 | 根因 | 修复 |
|------|------|------|
| 按复位有时能闭环有时不能 | 启动瞬间 TIM4 噪声捕获产生巨大 TempSpeed → 污染 HallSpeed | bValidTransition 跳过无效跳变 |
| 加 SpeedFd 就抖，不加就转 | SpeedRef=600 和 HallSpeed=2682 差太大 → PI 永远饱和在 ±3A | 切环时 SpeedRef = SpeedFd 零误差启动 |
| SpeedFd=HallSpeed 刹车，=-HallSpeed 加速 | 霍尔方向检测与 FOC 模型相反 | 修正 Direction 或接受负号但保证零误差 |
| bHallUpdated 在 if 中被清 0，后面 HallSpeed 不赋值 | 清 0 前没保存快照 | uint8_t hall_updated = bHallUpdated; 先保存 |

### 二、5 项霍尔信号修复

**① Hall_driev.h → 结构体加 bValidTransition**
```c
uint8_t bValidTransition;
```

**② HALL_Init_Electrical_Angle → 初始化所有变量**
```c
phandle->bValidTransition = 0;
phandle->AvrElSpeedDpp = 0.0f;
phandle->HallSpeed = 0.0f;
phandle->Direction = POSITIVE;
```

**③ 每个 case 设 bValidTransition**
合法跳转 → `phandle->bValidTransition = 1;`
非法跳转 → `phandle->bValidTransition = 0;`

**④ switch 后跳过无效跳变**
```c
if (!phandle->bValidTransition) return;
```

**⑤ hall_FOC_mode 中先保存 bHallUpdated**
```c
uint8_t hall_updated = HALL_Handle.bHallUpdated;
if (hall_updated) {
    // 角度同步...
    HALL_Handle.bHallUpdated = 0;
}
// 后续用 hall_updated 判断
```

### 三、速度环闭环的关键要素

**核心结论：** 速度环能不能闭环，不取决于 PI 参数、霍尔精度、方向正负——取决于闭环启动那一瞬间的误差。

**① 零误差启动（最关键）**
```c
HALL_rtU.SpeedRef = HALL_rtU.SpeedFd;  // 等于当前转速，误差 = 0
```

**② 速度斜坡**
```c
if (HALL_rtU.SpeedRef < 600.0f)
    HALL_rtU.SpeedRef += 0.5f;  // 每步爬 0.5 RPM
```

**③ 清空 PI 积分器**
```c
HALL_rtDW.Integrator_DSTATE = 0.0f;   // 速度环
HALL_rtDW.Integrator_DSTATE_o = 0.0f; // Id 电流环
HALL_rtDW.Integrator_DSTATE_l = 0.0f; // Iq 电流环
```

### 四、启动流程

```
上电 → 偏移校准(10次 ADC)
  → VF 开环拖动(4Hz, 12V, 1秒)
  → 切闭环条件满足(vf_count >= 10000):
      SpeedFd = HallSpeed
      SpeedRef = SpeedFd          // 零误差
      HALL_rtU.SpeedRef = SpeedFd // 零误差
      清空三个 PI 积分器
      HallElAngle = VF 角度       // 角度连续
      VF_flag = 1
  → hall_FOC_mode() 闭环运行:
      HallElAngle += AvrElSpeedDpp
      每步 SpeedRef += 0.5 → 600
      SpeedFd = HallSpeed
      FOC_Mode0_step()
      写 CCR
```

### 五、避坑指南

1. **不要用浮点判等做角度校准**：`== PHASE_SHIFT_ANGLE` 几乎永远不会触发。用 `bHallUpdated` 标志位。
2. **不要在主循环用 HAL_ADC_Stop**：会关掉 ADC 注入组，导致 FOC 停止。
3. **不要直接让 SpeedFd = SpeedFd**（笔误：不要直接让 SpeedRef = 固定值）：除非你从零误差开始 + 斜坡。
4. **VF 开环一定要设 Vbus**：`VF_rtU.Vbus = get_bus_voltage()`，否则除以零。
5. **HallSpeed 方向反了可以接受**：只要切环时 SpeedRef = SpeedFd，误差就是 0，正负不影响闭环建立。
6. **速度环 PI 增益太大没关系**：只要误差小，PI 在线性区就不会饱和振荡。

---

## 第6章：PWM 生成原理

### 6.1 什么是 CCR

**CCR = Capture/Compare Register（捕获/比较寄存器）**

每个 TIM 通道都有一个 CCR。在 PWM 输出模式下，它存放的是**占空比设定值**。

| 寄存器 | 含义 | 在你的项目中 | 作用 |
|--------|------|:-----------:|------|
| **ARR** | 自动重装载值 | 7999 | 决定 PWM **频率** |
| **CCR** | 比较值 | tABC[0/1/2] (0~8000) | 决定 PWM **占空比** |

计数器每步都和 CCR 比较：

```
中央对齐模式下：
  计数上升: CNT < CCR → 高电平，CNT ≥ CCR → 低电平
  计数下降: CNT > CCR → 高电平，CNT ≤ CCR → 低电平
```

| CCR | 占空比 | 波形示意 |
|:---:|:------:|----------|
| 0 | 0% | 一直低 |
| 2000 | 25% | ████░░░░ |
| 4000 | 50% | ████████ |
| 6000 | 75% | ████████░░░░ |
| 8000 | 100% | 一直高 |

### 6.2 PWM 生成的完整链路

```
FOC 算法 → tABC (Ud/Uq → SVPWM → 马鞍波)
         ↓
TIM1->CCR1/2/3 = tABC  (在 ADC 中断里直接写寄存器)
         ↓
TIM1 硬件比较器：CNT 和 CCR 逐拍比较
         ↓
6 路 PWM 输出到栅极驱动 FD6288T (PA8~PA10, PB13~PB15)
         ↓
FD6288T 驱动 6 个 MOS 管 (BSC0702LS) 三相全桥逆变
         ↓
电机三相绕组上产生旋转磁场 → 电机转动
```

### 6.3 你的项目中的 TIM1 配置

```c
Prescaler = 0                         // 170 MHz 不分频
Period    = 7999                       // ARR
CounterMode = CENTERALIGNED1          // 中央对齐模式
DeadTime  = 120                       // 死区 0.7μs，防止上下桥直通
CH1/2/3  → PWM 输出 (U/V/W 三相)
CH4      → ADC 触发 (TRGO = OC4REF)
```

```
PWM 频率 = 170 MHz / (2 × 8000) = 10.625 kHz
每个 PWM 周期 = 94 μs
```

### 6.4 引脚映射

| TIM1 通道 | GPIO | 驱动板接收端 | 相序 |
|-----------|------|-------------|:----:|
| CH1 | PA8 | HIN3（W 上桥） | W |
| CH1N | PB13 | LIN3（W 下桥） | W |
| CH2 | PA9 | HIN2（V 上桥） | V |
| CH2N | PB14 | LIN2（V 下桥） | V |
| CH3 | PA10 | HIN1（U 上桥） | U |
| CH3N | PB15 | LIN1（U 下桥） | U |

每对 CHx + CHxN 是互补输出，之间插入死区时间，防止上下 MOS 管同时导通短路。

---

## 第7章：无感 FOC 控制

### 7.1 为什么要做无感

在一些强磁、高温、潮湿、震动环境下，常规传感器的通信信号和寿命都会受到不同程度的影响。一旦传感器失效，依赖传感器作为信号反馈的闭环控制系统就无法启动。无感控制通过数学模型推算电机转子位置，无需依赖传感器，既节省成本又提高可靠性。

### 7.2 常见的无感控制算法

| 算法 | 原理 | 适用场景 |
|------|------|----------|
| 高频注入（HFI） | 注入高频电压信号，检测凸极效应引起的电流响应 | 零速/低速（带载启动） |
| 滑模观测器（SMO） | 构建滑模面，使观测电流逼近实际电流，反推反电动势 | 中高速，实现简单 |
| 龙伯格观测器（Luenberger） | 基于电机模型的全阶状态观测器 | 宽速度范围 |
| 磁链观测器 + PLL | 观测定子磁链，用 PLL 跟踪角度 | 中高速，Simulink 常用方案 |
| 模型参考自适应（MRAS） | 比较参考模型和可调模型的输出误差来估算 | 中高速 |

### 7.3 滑模观测器（SMO）基本原理

滑模观测器的核心思想是**构建一个新系统**，将原有电机系统中直接测量的量（电流、电压）作为新系统的输入信号，检测新系统的输出信号，并使重构系统在一定条件下与原系统相等。

### 7.4 电流观测器方程（SMO 核心数学模型）

电机的电流状态方程是 SMO 的起点。在 αβ 坐标系下，定子电流方程：

```
diα/dt = (-Rs × iα - eα + uα) / L
diβ/dt = (-Rs × iβ - eβ + uβ) / L
```

其中：
- uα, uβ：αβ 轴电压（已知，由上一拍 SVPWM 输出决定）
- iα, iβ：αβ 轴电流（已知，由 ADC 采样 + Clark 变换得到）
- eα, eβ：αβ 轴反电动势（未知，包含转子角度信息）
  - eα = -ωe × ψf × sinθe
  - eβ =  ωe × ψf × cosθe
- Rs, L：电机相电阻和相电感（已知参数）

SMO 的做法：**假设 eα = eβ = 0，用一个开关函数去补偿误差，迫使估算电流收敛到实测电流。**

### 7.5 SMO 完整计算流程（每步对应代码）

```
第1步：读电流 iα, iβ（ADC → Clark 变换）
第2步：计算估算电流 îα, îβ（观测器递推）
第3步：计算电流误差 Δiα = îα - iα，Δiβ = îβ - iβ
第4步：开关函数 zα = K × sign(Δiα)，zβ = K × sign(Δiβ)
第5步：更新下一拍估算电流：
    îα_new = îα + dt × [(-Rs × îα + uα - zα) / L]
    îβ_new = îβ + dt × [(-Rs × îβ + uβ - zβ) / L]
第6步：低通滤波 zα, zβ → 反电动势 êα, êβ
第7步：计算角度和速度：
    θ̂ = atan2(-êα, êβ)
    ω̂ = sqrt(êα² + êβ²) / ψf
```

### 7.6 SMO 的优缺点

| 优点 | 缺点 |
|------|------|
| 鲁棒性强，对 Rs/L 参数误差不敏感 | LPF 导致相位滞后，需补偿 |
| 实现简单，只有 sign + 积分 | 低速时反电动势小，信噪比差 |
| 无需额外硬件 | 开关噪声大，需要足够的 ADC 采样率 |
| 中高速表现好 | 零速/极低速无法工作（反电动势为零） |

> 你的项目里 yuge_FOC 例程用的不是 SMO，而是**磁链观测器 + PLL**方案，角度平滑性更好。SMO 的优势在于实现简单、参数不敏感，适合作为入门无感方案。，将原有电机系统中直接测量的量（电流、电压）作为新系统的输入信号，检测新系统的输出信号，并使重构系统在一定条件下与原系统相等。在电机控制中，将电机的电流和电压作为输入信号，通过滑模观测器估算出电机的角度和速度，然后作为电机控制系统的输入信号。

```
实际电机:  u, i  →  [真实转子位置 θ]
                          ↓
SMO 观测器: u, i  →  [估算反电动势 Eα, Eβ]  →  θ_est, ω_est
```
