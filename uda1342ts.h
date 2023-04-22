#ifndef __UDA1342TS_H_INCLUDED
#define __UDA1342TS_H_INCLUDED

#include "type.h"
#include "stdutils.h"

#define UDA1342TS_SLAVE_ADDR_DAC 0x1B
#define UDA1342TS_SLAVE_ADDR_ADC 0x1A
#define UDA1342TS_CLK 33868000


/*
*  Register mapping
 *  */
#define UDA1342TS_FUNC_SYSTEM 0x00
#define UDA1342TS_FUNC_SUB_SYSTEM 0x01
#define UDA1342TS_FUNC_DAC_FEATURES 0x10
#define UDA1342TS_FUNC_DAC_MASTER_VOLUME 0x11
#define UDA1342TS_FUNC_DAC_MIXER_VOLUME 0x12
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH1 0x20
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_CH2 0x21
#define UDA1342TS_FUNC_EVALUATION 0x30

/*
 *  UDA1342TS_FUNC_SYSTEM
 *  */
/* Reset bit */
#define UDA1342TS_FUNC_SYSTEM_N_RST 0x0 /* no reset */
#define UDA1342TS_FUNC_SYSTEM_RST (0x1 << 15) /* reset register to default */

/* Quick mode switch */
#define UDA1342TS_FUNC_SYSTEM_SOFT_MODE 0x0 /* soft mode change */
#define UDA1342TS_FUNC_SYSTEM_QUICK_MODE (0x1 << 14) /* quick mode change */

/* Mixer DC filtering */
#define UDA1342TS_FUNC_SYSTEM_MIXER_DC_FILTER_EN 0x0 /* enable mixer DC filtering */
#define UDA1342TS_FUNC_SYSTEM_MIXER_DC_FILTER_DIS (0x1 << 13) /* disable mixer DC filtering */

/* Outpute DC-filtering */
#define UDA1342TS_FUNC_SYSTEM_OUTPUT_DC_FILTER_DIS 0x0 /* disable output DC filtering */
#define UDA1342TS_FUNC_SYSTEM_OUTPUT_DC_FILTER_EN (0x1 << 12) /* enable output DC filtering */

/* ADC mode */
#define UDA1342TS_FUNC_SYSTEM_ADC_POWER_OFF 0x0	/* ADC power-off */
#define UDA1342TS_FUNC_SYSTEM_INPUT_SEL_1 (0x1 << 9) /* input 1 select (input 2 off) */
#define UDA1342TS_FUNC_SYSTEM_INPUT_SEL_2 (0x2 << 9) /* input 2 select (input 1 off) */
#define UDA1342TS_FUNC_SYSTEM_CH_SWAP_SIGNAL_INVER (0x4 << 9) /* channel swap and signal inversion */
#define UDA1342TS_FUNC_SYSTEM_INPUT_SEL_DOUBLE_DIFF_1 (0x5 << 9)/* input 1 select (double differential mode) */
#define UDA1342TS_FUNC_SYSTEM_INPUT_SEL_DOUBLE_DIFF_2 (0x6 << 9)/* input 2 select (double differential mode) */
#define UDA1342TS_FUNC_SYSTEM_MIXING_MODE (0x7 << 9) /* mixing mode */
#define UDA1342TS_FUNC_SYSTEM_ADC_MODE_MASK (0x7 << 9)

/* ADC polarity */
#define UDA1342TS_FUNC_SYSTEM_ADC_N_INVERT 0x0 /* non inverting */
#define UDA1342TS_FUNC_SYSTEM_ADC_INVERT (0x1 << 8) /* inverting */

/* System clock frequency settings */
#define UDA1342TS_FUNC_SYSTEM_256FS 0x0 /* 256fs */
#define UDA1342TS_FUNC_SYSTEM_384FS (0x1 << 5) /* 384fs */
#define UDA1342TS_FUNC_SYSTEM_512FS (0x2 << 5) /* 512fs */
#define UDA1342TS_FUNC_SYSTEM_768FS (0x3 << 5) /* 768fs */
#define UDA1342TS_FUNC_SYSTEM_SYS_CLK_MASK (0x3 << 5)

/* Data format */
#define UDA1342TS_FUNC_SYSTEM_I2S_BUS 0x0 /* i2s-bus */
#define UDA1342TS_FUNC_SYSTEM_LSB_16 (0x1 << 2)	/* LSB-justified 16 bits */
#define UDA1342TS_FUNC_SYSTEM_LSB_20 (0x2 << 2)	/* LSB-justified 20 bits */
#define UDA1342TS_FUNC_SYSTEM_LSB_24 (0x3 << 2)	/* LSB-justified 24 bits */
#define UDA1342TS_FUNC_SYSTEM_MSB (0x4 << 2)	/* MSB-justified */
#define UDA1342TS_FUNC_SYSTEM_LSB_IN_16_MSB_OUT (0x5 << 2) /* LSB-justified 16 bits input and MSB-justified output */
#define UDA1342TS_FUNC_SYSTEM_LSB_IN_20_MSB_OUT (0x6 << 2) /* LSB-justified 20 bits input and MSB-justified output */
#define UDA1342TS_FUNC_SYSTEM_LSB_IN_24_MSB_OUT (0x7 << 2) /* LSB-justified 24 bits input and MSB-justified output */
#define UDA1342TS_FUNC_SYSTEM_DATA_FORMAT_MASK (0x7 << 2)

/* DAC power control */
#define UDA1342TS_FUNC_SYSTEM_DAC_POWER_OFF 0x0	/* DAC power-off */
#define UDA1342TS_FUNC_SYSTEM_DAC_POWER_ON (0x1 << 1) /* DAC power-on */

/* Polarity control of DAC */
#define UDA1342TS_FUNC_SYSTEM_DAC_N_INVERT 0x0 /* non inverting */
#define UDA1342TS_FUNC_SYSTEM_DAC_INVERT 0x1 /* inverting */

/*
 *  Sub System
 *  */
/* Input oversampleing rate */
#define UDA1342TS_FUNC_SUB_SYSTEM_SINGLE_SPEED 0x0 /* single speed */
#define UDA1342TS_FUNC_SUB_SYSTEM_DOUBLE_SPEED (0x1 << 6) /* double speed */
#define UDA1342TS_FUNC_SUB_SYSTEM_QUAD_SPEED (0x2 << 6)	/* quad speed */
#define UDA1342TS_FUNC_SUB_SYSTEM_OVERSAMPLING_RATE_MASK (0x3 << 6)

/* DAC mixing position switch */
#define UDA1342TS_FUNC_SUB_SYSTEM_BEFORE_SND_FEATURE 0x0 /* before sound features */
#define UDA1342TS_FUNC_SUB_SYSTEM_AFTER_SND_FEATURE (0x1 << 5) /* after sound features */

/* DAC mixer */
#define UDA1342TS_FUNC_SUB_SYSTEM_DAC_MIXER_DIS 0x0 /* disable mixer */
#define UDA1342TS_FUNC_SUB_SYSTEM_DAC_MIXER_EN (0x1 << 4) /* enable mixer */ 

/* Slience detection period */
#define UDA1342TS_FUNC_SUB_SYSTEM_3200_SAMPLES 0x0 /* 3200 samples */
#define UDA1342TS_FUNC_SUB_SYSTEM_4800_SAMPLES (0x1 << 2) /* 4800 samples */
#define UDA1342TS_FUNC_SUB_SYSTEM_9600_SAMPLES (0x2 << 2) /* 9600 samples */
#define UDA1342TS_FUNC_SUB_SYSTEM_19200_SAMPLES (0x3 << 2) /* 19200 samples */
#define UDA1342TS_FUNC_SUB_SYSTEM_SLIENCE_DET_PERIOD_MASK (0x3 << 2)

/* Multi purpose output selection */
#define UDA1342TS_FUNC_SUB_SYSTEM_NO_OUTPUT 0x0	/* no output */
#define UDA1342TS_FUNC_SUB_SYSTEM_OVR_ADC_DET 0x1 /* overflow (ADC) detection */
#define UDA1342TS_FUNC_SUB_SYSTEM_DIGIT_SLIENCE_DET 0x3	/* digitial silence detection */
#define UDA1342TS_FUNC_SUB_SYSTEM_MULTI_PURPOSE_OUTPUT_SEL_MASK 0x3

/*
 *  DAC features
 *  */
/* Flat/min/max. switch position */
#define UDA1342TS_FUNC_DAC_FEATURES_FLAT 0x0 /* flat */
#define UDA1342TS_FUNC_DAC_FEATURES_MIN (0x1 << 14) /* min */
#define UDA1342TS_FUNC_DAC_FEATURES_MAX (0x3 << 14) /* max */
#define UDA1342TS_FUNC_DAC_FEATURES_MODE_MASK (0x3 << 14)

/* Bass boost settings */
#define UDA1342TS_FUNC_DAC_FEATURES_BASS_BOOST(x) (((x) & 0xf) << 10)
#define UDA1342TS_FUNC_DAC_FEATURES_BASS_BOOST_MASK (0xf << 10)

/* Treble settings */
#define UDA1342TS_FUNC_DAC_FEATURES_TREBLE(x) (((x) & 0x3) << 8)
#define UDA1342TS_FUNC_DAC_FEATURES_TREBLE_MASK (0x3 << 8)

/* Slience detector switch */
#define UDA1342TS_FUNC_DAC_FEATURES_SILENCE_DET_DiS 0x0	/* disable silence detector */
#define UDA1342TS_FUNC_DAC_FEATURES_SILENCE_DET_EN (0x1 << 7) /* enable silence detector */

/* Mute DAC */
#define UDA1342TS_FUNC_DAC_FEATURES_ADC_N_MUTE 0x0 /* no DAC muting */
#define UDA1342TS_FUNC_DAC_FEATURES_ADC_MUTE (0x1 << 6) /* DAC muting */

/* Mute ADC */
#define UDA1342TS_FUNC_DAC_FEATURES_DAC_N_MUTE 0x0 /* no ADC muting */
#define UDA1342TS_FUNC_DAC_FEATURES_DAC_MUTE (0x1 << 5) /* ADC muting */

/* Mute */
#define UDA1342TS_FUNC_DAC_FEATURES_N_MUTE 0x0 /* no muting */
#define UDA1342TS_FUNC_DAC_FEATURES_MUTE (0x1 << 4) /* muting */

/* Quick mute mode settings */
#define UDA1342TS_FUNC_DAC_FEATURES_SOFT_MUTE_MODE 0x0 /* soft mute mode */
#define UDA1342TS_FUNC_DAC_FEATURES_QUICK_MUTE_MODE (0x1 << 3) /* quick mute mode */

/* De-emphasis settings */
#define UDA1342TS_FUNC_DAC_FEATURES_N_DE_EMPHASIS 0x0 /* no de-emphasis */
#define UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_32KHZ 0x1 /* de-emphasis at fs= 32kHz */
#define UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_44_1KHZ 0x2 /* de-emphasis at fs= 44.1kHz */
#define UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_48KHZ 0x3 /* de-emphasis at fs= 48kHz */
#define UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_96KHZ 0x4 /* de-emphasis at fs= 96kHz */
#define UDA1342TS_FUNC_DAC_FEATURES_DE_EMPHASIS_MASK 0x7

/*
 *  ADC input and mixer gain channel
 *  */
/* ADC input amplifier gain settings */
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_INPUT(x) (((x) & 0xf) << 8)
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_INPUT_MASK (0xf << 8)

/* ADC mixer gain settings */
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_MIXER(x) ((x) & 0xff)
#define UDA1342TS_FUNC_ADC_INPUT_MIXER_GAIN_MIXER_MASK 0xff

extern int uda1342ts_dac_initialize(void);
extern int uda1342ts_adc_initialize(void);
extern int uda1342ts_dac_master_volume(uint16_t val);
extern int uda1342ts_dac_mixer_volume(uint16_t val);
extern int uda1342ts_dac_enable_mixer(Boolean_et val);
extern int uda1342ts_dac_mode(uint16_t mode);
extern int uda1342ts_dac_bass_boost(uint8_t val);
extern int uda1342ts_dac_treble(uint8_t val);
extern int uda1342ts_dac_mute(Boolean_et val);
extern int uda1342ts_dac_de_emphasis(uint16_t val);
extern int uda1342ts_adc_mode(uint16_t val);
extern int uda1342ts_adc_input_amplifier_gain(uint8_t reg, uint16_t val);
extern int uda1342ts_adc_mixer_gain(uint8_t reg, uint16_t val);
extern int uda1342ts_output_dc_filter(Boolean_et val);
extern int uda1342ts_mixer_dc_filter(Boolean_et val);
extern int uda1342ts_set_input_channel(uint32_t ch);

#endif
