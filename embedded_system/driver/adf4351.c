/**
 * @author hzy
 * @section DESCRIPTION
 * 
 * ADF4351 driver library.
 */

#include "adf4351.h"
#include <math.h>
#define VCO_MAX_FREQUENCY 4400.
#define VCO_MIN_FREQUENCY 2200.

void (*adf4351_spi_transmit)(uint32_t word);

/**
 * 辗转相除法求最大公约数
 */
uint32_t GCD(uint32_t a, uint32_t b) {
  return a == 0 ? b : GCD(b % a, a);
}

/**
 * 初始化ADF4351
 * @param spi_transmit SPI回调函数，使用SPI模式0，只需要发送，发送前将LE拉低，发送完成后拉高。
 * 注意整个数据都是大端模式，而大部分处理器是小端模式，所以可能需要转换。
 * @param word 4字节的数据
 */
void adf4351_init(void (*spi_transmit)(uint32_t word)) {
  adf4351_spi_transmit = spi_transmit;
}

/**
 * 写寄存器
 * @param word 寄存器值，包含控制位
 */
void adf4351_write_reg(uint32_t word) {
  adf4351_spi_transmit(word);
}

/**
 * 通过配置值计算ADF4351参数值，这个函数不会验证配置是否合理，也不会修改配置的值，即使配置值是错误的
 * @param cfg ADF4351配置结构体的指针
 * @return ADF4351参数结构体
 */
adf4351_parameter_t adf4351_calculate_parameter(const adf4351_config_t *cfg) {
  adf4351_parameter_t param;
  // output frequency should be less than 4400MHz and grater than 35MHz, VCO frequency is 2200 to 4400
  if (cfg->output_frequency > VCO_MIN_FREQUENCY) {
    param.output_divider = OUTPUT_DIVIDER_1;
  } else if (cfg->output_frequency > VCO_MIN_FREQUENCY / 2) {
    param.output_divider = OUTPUT_DIVIDER_2;
  } else if (cfg->output_frequency > VCO_MIN_FREQUENCY / 4) {
    param.output_divider = OUTPUT_DIVIDER_4;
  } else if (cfg->output_frequency > VCO_MIN_FREQUENCY / 8) {
    param.output_divider = OUTPUT_DIVIDER_8;
  } else if (cfg->output_frequency > VCO_MIN_FREQUENCY / 16) {
    param.output_divider = OUTPUT_DIVIDER_16;
  } else if (cfg->output_frequency > VCO_MIN_FREQUENCY / 32) {
    param.output_divider = OUTPUT_DIVIDER_32;
  } else {
    param.output_divider = OUTPUT_DIVIDER_64;
  }
  // PFD
  const float pfd_frequency = cfg->reference_frequency * (cfg->ref_double + 1) / (cfg->ref_div2 + 1) / cfg->r_counter;
  // 反馈频率
  const float feedback_frequency = cfg->feedback_select == FEEDBACK_SIGNAL_FUNDAMENTAL ? (cfg->output_frequency * (1 << param.output_divider)) : cfg->output_frequency;
  // N=反馈频率/PFD
  const float N = feedback_frequency / pfd_frequency;
  param.int_value = (uint16_t)N;
  // 四舍五入取整
  param.mod_value = roundf(pfd_frequency * 1e3 / cfg->channel_spacing);
  // 保证MOD < 0xfff
  if (param.mod_value > 0xfff) {
    param.mod_value = 0xfff;
  }
  // 四舍五入取整
  param.frac_value = round(param.mod_value * (N - param.int_value));
  // 约分
  const uint32_t gcd = GCD(param.frac_value, param.mod_value);
  param.frac_value /= gcd;
  param.mod_value /= gcd;

  // INT必须大于等于75(prescaler=8/9)或23(prescaler=4/5)
  if (cfg->prescaler == PRESCALER_8_9 && param.int_value < 75) {
    param.int_value = 75;
  }
  if (cfg->prescaler == PRESCALER_4_5 && param.int_value < 23) {
    param.int_value = 23;
  }
  return param;
}

/**
 * 获得真实频率
 * @param cfg ADF4351配置结构体的指针
 * @param param ADF4351参数结构体的指针
 * @return 真实的频率，单位: MHz
 */
float adf4351_get_actual_frequency(const adf4351_config_t *cfg, const adf4351_parameter_t *param) {
  float pfd_frequency = cfg->reference_frequency * (cfg->ref_double + 1) / (cfg->ref_div2 + 1);
  if (cfg->r_counter) {
    pfd_frequency /= cfg->r_counter;
  }
  return pfd_frequency * (param->int_value + (float)param->frac_value / param->mod_value) / (1u << param->output_divider);
}

/**
 * 生成寄存器值
 * @param reg_num 寄存器号，0 ~ 5
 * @param cfg ADF4351配置结构体的指针
 * @param param ADF4351参数结构体的指针
 * @return 寄存器值
 */
uint32_t adf4351_generate_register(uint8_t reg_num, const adf4351_config_t *cfg, const adf4351_parameter_t *param) {
  uint32_t value = reg_num;
  switch (reg_num) {
  case 0: {
    value |= param->int_value << 15u;
    value |= param->frac_value << 3u;
    break;
  }
  case 1: {
    value |= cfg->phase_adjust << 28u;
    value |= cfg->prescaler << 27u;
    value |= cfg->phase_value << 15u;
    value |= param->mod_value << 3u;
    break;
  }
  case 2: {
    value |= cfg->low_noise_spur_mode << 29u;
    value |= cfg->muxout << 26u;
    value |= cfg->ref_double << 25u;
    value |= cfg->ref_div2 << 24u;
    value |= cfg->r_counter << 14u;
    value |= cfg->double_buffer << 13u;
    value |= cfg->charge_pump_current << 9u;
    value |= cfg->ldf << 8u;
    value |= cfg->ldp << 7u;
    value |= cfg->pd_polarity << 6u;
    value |= cfg->powerdown << 5u;
    value |= cfg->cp_3_state << 4u;
    value |= cfg->counter_reset << 3u;
    break;
  }
  case 3: {
    value |= cfg->band_select_clock_mode << 23u;
    value |= cfg->abp << 22u;
    value |= cfg->charge_cancellation << 21u;
    value |= cfg->csr << 18u;
    value |= cfg->clock_div_mode << 15u;
    value |= cfg->clock_divider_value << 3u;
    break;
  }
  case 4: {
    value |= cfg->feedback_select << 23u;
    value |= param->output_divider << 20u;
    value |= cfg->band_select_clock_divider << 12u;
    value |= cfg->vco_powerdown << 11u;
    value |= cfg->mtld << 10u;
    value |= cfg->aux_output_select << 9u;
    value |= cfg->aux_output_enable << 8u;
    value |= cfg->aux_output_power << 6u;
    value |= cfg->rf_output_enable << 5u;
    value |= cfg->rf_output_power << 3u;
    break;
  }
  case 5: {
    value |= 0x180000u; // reserved
    value |= cfg->ld_pin_mode << 22u;
    break;
  }
  default: {
    // unknow reg_num, set to 0
    reg_num = 0;
    break;
  }
  }
  return value;
}
