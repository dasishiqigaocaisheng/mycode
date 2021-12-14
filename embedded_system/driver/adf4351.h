/**
 * @author hzy
 * @section DESCRIPTION
 * 
 * ADF4351 driver library header file.
 */

#pragma once
#include <stdint.h>

typedef enum {
  PRESCALER_4_5 = 0,
  PRESCALER_8_9
} prescaler_t;

typedef enum {
  FEEDBACK_SIGNAL_DIVIDED = 0,
  FEEDBACK_SIGNAL_FUNDAMENTAL
} feedback_select_t;

typedef enum {
  LOW_NOISE_MODE = 0,
  LOW_SPUR_MODE = 3
} low_noise_spur_mode_t;

typedef enum {
  MUXOUT_3_STATE = 0,
  MUXOUT_DVDD,
  MUTOUT_DGND,
  MUXOUT_R_DIVIDER,
  MUXOUT_N_DIVIDER,
  MUXOUT_ANALOG_LOCK,
  MUXOUT_DIGITAL_LOCK
} muxout_t;

typedef enum {
  CHARGE_PUMP_CURRENT_0_31 = 0,
  CHARGE_PUMP_CURRENT_0_63,
  CHARGE_PUMP_CURRENT_0_94,
  CHARGE_PUMP_CURRENT_1_25,
  CHARGE_PUMP_CURRENT_1_56,
  CHARGE_PUMP_CURRENT_1_88,
  CHARGE_PUMP_CURRENT_2_19,
  CHARGE_PUMP_CURRENT_2_50,
  CHARGE_PUMP_CURRENT_2_81,
  CHARGE_PUMP_CURRENT_3_13,
  CHARGE_PUMP_CURRENT_3_44,
  CHARGE_PUMP_CURRENT_3_75,
  CHARGE_PUMP_CURRENT_4_06,
  CHARGE_PUMP_CURRENT_4_38,
  CHARGE_PUMP_CURRENT_4_69,
  CHARGE_PUMP_CURRENT_5_00
} charge_pump_current_t;

typedef enum {
  LDF_FRAC_N = 0,
  LDF_INT_N
} ldf_t;

typedef enum {
  LDP_10ns = 0,
  LDP_6ns
} ldp_t;

typedef enum {
  PD_POLARITY_NEGTIVE = 0,
  PD_POLARITY_POSTIVE
} pd_polarity_t;

typedef enum {
  BAND_SELECT_CLOCK_MODE_LOW = 0,
  BAND_SELECT_CLOCK_MODE_HIGH
} band_select_clock_mode_t;

typedef enum {
  ABP_6ns = 0,
  ABP_3ns
} abp_t;

typedef enum {
  CLOCK_DIV_MODE_CLOCK_DIVIDER_OFF = 0,
  CLOCK_DIV_MODE_FAST_LOCK_ENABLE,
  CLOCK_DIV_MODE_RESYNC_ENABLE
} clock_div_mode_t;

typedef enum {
  AUX_OUTPUT_SELECT_DIVIDED = 0,
  AUX_OUTPUT_SELECT_FUNDAMENTAL
} aux_output_select_t;

typedef enum {
  AUX_OUTPUT_POWER_neg_4 = 0,
  AUX_OUTPUT_POWER_neg_1,
  AUX_OUTPUT_POWER_pos_2,
  AUX_OUTPUT_POWER_pos_5
} aux_output_power_t;

typedef enum {
  RF_OUTPUT_POWER_neg_4 = 0,
  RF_OUTPUT_POWER_neg_1,
  RF_OUTPUT_POWER_pos_2,
  RF_OUTPUT_POWER_pos_5
} rf_output_power_t;

typedef enum {
  LD_PIN_MODE_LOW = 0,
  LD_PIN_MODE_DIGITAL_LOCK_DETECT = 1,
  LD_PIN_MODE_HIGH = 3
} ld_pin_mode_t;

typedef struct {
  // RF setting
  // 输入频率, 单位: MHz
  uint32_t reference_frequency;
  // 输出频率，单位: MHz
  float output_frequency;
  // 频率间隔，单位: KHz
  float channel_spacing;
  // 输入R分频，1 ~ 1023
  uint16_t r_counter;
  // 输入2倍频，0: 关闭, 1: 开启
  uint8_t ref_double;
  // 输入2分频，0: 关闭, 1: 开启
  uint8_t ref_div2;
  // 预分频，0: 4/5, 1: 8/9
  prescaler_t prescaler;
  // 反馈选择，0: 分频后, 1: 原始
  feedback_select_t feedback_select;
  // 相位调整，0: 关闭, 1: 开启
  uint8_t phase_adjust;
  // 相位值，0 ~ 4096
  uint16_t phase_value;
  // register 2
  // 低噪声和低杂散模式
  low_noise_spur_mode_t low_noise_spur_mode;
  // 多路复用
  muxout_t muxout;
  // 双缓冲，0: 关闭, 1: 开启
  uint8_t double_buffer;
  // 电荷泵电流
  charge_pump_current_t charge_pump_current;
  // 锁定检测功能，对于小数N分频模式，建议设为0;对于整数N分频模式，建议设为1
  ldf_t ldf;
  // 锁定检测精度，对于小数N分频模式，建议设为0;对于整数N分频模式，建议设为1
  ldp_t ldp;
  // 鉴相器极性
  pd_polarity_t pd_polarity;
  // 关断，0: 正常工作，1: 关断
  uint8_t powerdown;
  // 电荷泵三态，设置为1时，电荷泵进入三态模式。正常工作时，应设置为0
  uint8_t cp_3_state;
  // 分频器复位，当设为1时，RF频率合成器N分频器和R分频器处于复位状态。正常工作时，应设置为0
  uint8_t counter_reset;
  // register 3
  // 频段选择时钟模式
  band_select_clock_mode_t band_select_clock_mode;
  // 防反冲脉冲宽度，设为1时，PFD防反冲脉冲宽度为3ns，可改善整数N分频操作的相位噪声和杂散性能。对于小数N分频操作，不建议使用3ns设置
  abp_t abp;
  // 电荷消除，设为1将使能电荷泵电荷消除功能，这可以降低整 数N分频模式下的PFD杂散。在小数N分频模式下，应设置为0
  uint8_t charge_cancellation;
  // 周跳减少使能，0关闭，1开启
  uint8_t csr;
  // 时钟分频器模式
  clock_div_mode_t clock_div_mode;
  // 时钟分频器值，0 ～ 4095
  uint16_t clock_divider_value;
  // register 4
  // VCO关断，0上电，1关断
  uint8_t vco_powerdown;
  // 静音至检测到锁定，如果设置为1，则切断RF输出级的电源电流，直到数字锁定检测电路检测到器件实现锁定为止
  uint8_t mtld;
  // 辅助输出选择
  aux_output_select_t aux_output_select;
  // 辅助输出使能，0禁用输出，1启用输出
  uint8_t aux_output_enable;
  // 辅助输出功率
  aux_output_power_t aux_output_power;
  // RF输出使能，0禁用输出，1启用输出
  uint8_t rf_output_enable;
  // RF输出功率
  rf_output_power_t rf_output_power;
  // 频段选择时钟分频器值，1 ~ 255，R分频器的输出默认用作频段选择逻辑时钟，但如果此值太大(>125 kHz)，则可以启用一个分频器，以将R分频器输出细分为较小的值
  uint8_t band_select_clock_divider;
  // register 5
  // 锁定检测引脚工作方式
  ld_pin_mode_t ld_pin_mode;
} adf4351_config_t;

typedef enum {
  OUTPUT_DIVIDER_1 = 0,
  OUTPUT_DIVIDER_2,
  OUTPUT_DIVIDER_4,
  OUTPUT_DIVIDER_8,
  OUTPUT_DIVIDER_16,
  OUTPUT_DIVIDER_32,
  OUTPUT_DIVIDER_64,
} output_divider_t;

typedef struct {
  // register 0

  uint16_t int_value;
  uint16_t frac_value;
  // register 1

  uint16_t mod_value;
  // register 4

  output_divider_t output_divider;
} adf4351_parameter_t;

void adf4351_init(void (*spi_transmit)(uint32_t word));
void adf4351_write_reg(uint32_t word);
adf4351_parameter_t adf4351_calculate_parameter(const adf4351_config_t *cfg);
uint32_t adf4351_generate_register(uint8_t reg_num, const adf4351_config_t *cfg, const adf4351_parameter_t *param);
float adf4351_get_actual_frequency(const adf4351_config_t *cfg, const adf4351_parameter_t *param);
