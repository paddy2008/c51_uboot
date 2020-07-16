#ifndef __100g_sfp_EFM8LB1_qsfp28_h__
#define __100g_sfp_EFM8LB1_qsfp28_h__

#include <SI_EFM8LB1_Register_Enums.h>

/******Select Page Macro Define*****/
#define SFP_PAGE_SELECT		(0x7F)

/*****Scale For Temp Calculations*****/
#define DDMI_CALI_TEMP_SCALE	(0x100)

#define MAX_CHANNEL_NUMS        (0x4)


/********************************************Lower Page Addr Mapping Tab******************************/
#define MSA_BOOT_STATUS		(0x2)
#define MSA_TX_ENABLE		(86)

/********************************************MSA Page2 Addr Mapping Tab*******************************/
#define SFP_DEBUG_LOG_START_ADDR  (0x80)

/********************************************MSA Page3 Addr Mapping Tab*******************************/

#define PAGE3_TEMP_HIGH_ALARM_MSB         (0x80)
#define PAGE3_TEMP_HIGH_ALARM_LSB         (0x81)
#define PAGE3_TEMP_LOW_ALARM_MSB          (0x82)
#define PAGE3_TEMP_LOW_ALARM_LSB          (0x83)
#define PAGE3_TEMP_HIGH_WARN_MSB          (0x84)
#define PAGE3_TEMP_HIGH_WARN_LSB          (0x85)
#define PAGE3_TEMP_LOW_WARN_MSB           (0x86)
#define PAGE3_TEMP_LOW_WARN_LSB           (0x87)

#define PAGE3_VCC_HIGH_ALARM_MSB          (0x90)
#define PAGE3_VCC_HIGH_ALARM_LSB          (0x91)
#define PAGE3_VCC_LOW_ALARM_MSB           (0x92)
#define PAGE3_VCC_LOW_ALARM_LSB           (0x93)
#define PAGE3_VCC_HIGH_WARN_MSB           (0x94)
#define PAGE3_VCC_HIGH_WARN_LSB           (0x95)
#define PAGE3_VCC_LOW_WARN_MSB            (0x96)
#define PAGE3_VCC_LOW_WARN_LSB            (0x97)

#define PAGE3_RXPWR_HIGH_ALARM_MSB        (0xB0)
#define PAGE3_RXPWR_HIGH_ALARM_LSB        (0xB1)
#define PAGE3_RXPWR_LOW_ALARM_MSB         (0xB2)
#define PAGE3_RXPWR_LOW_ALARM_LSB         (0xB3)
#define PAGE3_RXPWR_HIGH_WARN_MSB         (0xB4)
#define PAGE3_RXPWR_HIGH_WARN_LSB         (0xB5)
#define PAGE3_RXPWR_LOW_WARN_MSB          (0xB6)
#define PAGE3_RXPWR_LOW_WARN_LSB          (0xB7)

#define PAGE3_TXBIAS_HIGH_ALARM_MSB       (0xB8)
#define PAGE3_TXBIAS_HIGH_ALARM_LSB       (0xB9)
#define PAGE3_TXBIAS_LOW_ALARM_MSB        (0xBA)
#define PAGE3_TXBIAS_LOW_ALARM_LSB        (0xBB)
#define PAGE3_TXBIAS_HIGH_WARN_MSB        (0xBC)
#define PAGE3_TXBIAS_HIGH_WARN_LSB        (0xBD)
#define PAGE3_TXBIAS_LOW_WARN_MSB         (0xBE)
#define PAGE3_TXBIAS_LOW_WARN_LSB         (0xBF)

#define PAGE3_TXPWR_HIGH_ALARM_MSB        (0xC0)
#define PAGE3_TXPWR_HIGH_ALARM_LSB        (0xC1)
#define PAGE3_TXPWR_LOW_ALARM_MSB         (0xC2)
#define PAGE3_TXPWR_LOW_ALARM_LSB         (0xC3)
#define PAGE3_TXPWR_HIGH_WARN_MSB         (0xC4)
#define PAGE3_TXPWR_HIGH_WARN_LSB         (0xC5)
#define PAGE3_TXPWR_LOW_WARN_MSB          (0xC6)
#define PAGE3_TXPWR_LOW_WARN_LSB          (0xC7)

/********************************************MSA Page4 Addr Mapping Tab*******************************/

#define SFP_DDMI_TEMP_MSB					(0x80)
#define SFP_DDMI_TEMP_LSB					(0x81)

#define SFP_DDMI_VCC_MSB					(0x82)
#define SFP_DDMI_VCC_LSB					(0x83)

#define SFP_DDMI_IBIAS0_MSB					(0x84)
#define SFP_DDMI_IBIAS0_LSB					(0x85)

#define SFP_DDMI_RXPOWER0_MSB				(0x86)
#define SFP_DDMI_RXPOWER0_LSB				(0x87)

#define SFP_DDMI_TXPOWER0_MSB				(0x88)
#define SFP_DDMI_TXPOWER0_LSB				(0x89)

#define SFP_CUR_BIAS0_MSB					(0x8A)
#define SFP_CUR_BIAS0_LSB					(0x8B)

#define SFP_CUR_MOD0_MSB					(0x8C)
#define SFP_CUR_MOD0_LSB					(0x8D)

#define SFP_CUR_CROSSING0_MSB				(0x8E)
#define SFP_CUR_CROSSING0_LSB				(0x8F)

#define SFP_DDMI_IBIAS1_MSB					(0x90)
#define SFP_DDMI_IBIAS1_LSB					(0x91)

#define SFP_DDMI_RXPOWER1_MSB				(0x92)
#define SFP_DDMI_RXPOWER1_LSB				(0x93)

#define SFP_DDMI_TXPOWER1_MSB				(0x94)
#define SFP_DDMI_TXPOWER1_LSB				(0x95)

#define SFP_CUR_BIAS1_MSB					(0x96)
#define SFP_CUR_BIAS1_LSB					(0x97)

#define SFP_CUR_MOD1_MSB					(0x98)
#define SFP_CUR_MOD1_LSB					(0x99)

#define SFP_CUR_CROSSING1_MSB				(0x9A)
#define SFP_CUR_CROSSING1_LSB				(0x9B)

#define SFP_DDMI_IBIAS2_MSB					(0x9C)
#define SFP_DDMI_IBIAS2_LSB					(0x9D)

#define SFP_DDMI_RXPOWER2_MSB				(0x9E)
#define SFP_DDMI_RXPOWER2_LSB				(0x9F)

#define SFP_DDMI_TXPOWER2_MSB				(0xA0)
#define SFP_DDMI_TXPOWER2_LSB				(0xA1)

#define SFP_CUR_BIAS2_MSB					(0xA2)
#define SFP_CUR_BIAS2_LSB					(0xA3)

#define SFP_CUR_MOD2_MSB					(0xA4)
#define SFP_CUR_MOD2_LSB					(0xA5)

#define SFP_CUR_CROSSING2_MSB				(0xA6)
#define SFP_CUR_CROSSING2_LSB				(0xA7)

#define SFP_DDMI_IBIAS3_MSB					(0xA8)
#define SFP_DDMI_IBIAS3_LSB					(0xA9)

#define SFP_DDMI_RXPOWER3_MSB				(0xAA)
#define SFP_DDMI_RXPOWER3_LSB				(0xAB)

#define SFP_DDMI_TXPOWER3_MSB				(0xAC)
#define SFP_DDMI_TXPOWER3_LSB				(0xAD)

#define SFP_CUR_BIAS3_MSB					(0xAE)
#define SFP_CUR_BIAS3_LSB					(0xAF)

#define SFP_CUR_MOD3_MSB					(0xB0)
#define SFP_CUR_MOD3_LSB					(0xB1)

#define SFP_CUR_CROSSING3_MSB				(0xB2)
#define SFP_CUR_CROSSING3_LSB				(0xB3)

#define SFP_PIN_STATUS						(0xB4)

#define MSA_PAGE4_TEC_TEMP_READBACK_MSB		(0xb5)
#define MSA_PAGE4_TEC_TEMP_READBACK_LSB		(0xb6)

#define MSA_PAGE4_TEC_TEMP_SET_MSB			(0xb7)
#define MSA_PAGE4_TEC_TEMP_SET_LSB			(0xb8)

#define MSA_PAGE4_ITEC_MSB					(0xb9)
#define MSA_PAGE4_ITEC_LSB					(0xba)

#define MSA_PAGE4_VTEC_MSB					(0xbb)
#define MSA_PAGE4_VTEC_LSB					(0xbc)

#define SFP_MOD_BOOT_REASON				    (0xBD)

#define SFP_JUMP_TO_BOOT					(0xDA)
#define SFP_DBG_CMD							(0xDB)
		#define SFP_DBG_CMD_IDLE 	(0xF2)
#define SFP_DBG_CMD_STATUS					(0xDC)
#define SFP_DBG_CMD_I2C_DEV_ADDR			(0xDD)
#define SFP_DBG_CMD_OFFSET_ADDR_LSB			(0xDE)
#define SFP_DBG_CMD_OFFSET_ADDR_MSB			(0xDF)

#define SFP_DBG_READ_DATA0					(0xE0)
#define SFP_DBG_READ_DATA1					(0xE1)
#define SFP_DBG_READ_DATA2					(0xE2)
#define SFP_DBG_READ_DATA3					(0xE3)
#define SFP_DBG_READ_DATA4					(0xE4)
#define SFP_DBG_READ_DATA5					(0xE5)
#define SFP_DBG_READ_DATA6					(0xE6)
#define SFP_DBG_READ_DATA7					(0xE7)
#define SFP_DBG_READ_DATA8					(0xE8)
#define SFP_DBG_READ_DATA9					(0xE9)
#define SFP_DBG_READ_DATA10					(0xEA)
#define SFP_DBG_READ_DATA11					(0xEB)
#define SFP_DBG_READ_DATA12					(0xEC)
#define SFP_DBG_READ_DATA13					(0xED)
#define SFP_DBG_READ_DATA14					(0xEE)
#define SFP_DBG_READ_DATA15					(0xEF)

#define SFP_MOD_SECUR_LEVEL_STATUS			(0xF0)
#define SFP_DDMI_ENABLE						(0xF1)
#define SFP_AUTO_ENABLE						(0xF2)
#define SFP_MOD_REA_SEC						(0xF3)
#define SFP_MOD_REA_MIN						(0xF4)
#define SFP_MOD_REA_HOUR					(0xF5)
#define SFP_MOD_REA_DAY						(0xF6)
#define SFP_MOD_REA_MON						(0xF7)
#define SFP_MOD_REA_YEAR				    (0xF8)
#define SFP_MOD_REA_VERSION					(0xF9)
#define SFP_MOD_TYPE						(0xFA)
#define SFP_BOOT_STATUS						(0xFB)


/**************************************************MSA Page5 Address Define***************************/
#define MSA_VENDOR_PN					(0xA8)
#define MSA_VENDOR_PN_LEN				(16)
#define MSA_VENDOR_REV					(0xB8)
#define MSA_VENDOR_REV_LEN				(2)
#define MSA_VENDOR_SN					(0xC4)
#define MSA_VENDOR_SN_LEN				(16)
#define MSA_DATA_CODE					(0xD4)
#define MSA_DATA_CODE_LEN				(8)
#define MSA_MODULE_CLASS				(0XDE)
#define MSA_OFF_MODULE_TYPE				(0x80)
#define MSA_OFF_MODULE_SUB_TYPE			(0x81)
#define MSA_OFF_HW_VER					(0x82)
#define PCBA_CODE						(0x83)  //length = 16byte
#define PCBA_CODE_LEN					(16)
#define LASER_TX_SN						(0x93)  //length = 16byte
#define LASER_TX_SN_LEN					(16)
#define LASER_RX_SN						(0xA3)  //length = 16byte
#define LASER_RX_SN_LEN					(16)
#define PCBA_SN							(0xB3)  //length = 12byte
#define PCBA_SN_LEN						(16)
#define SUPERCODE						(0xC3)  //length = 9 byte
#define SUPERCODE_LEN					(9)
#define PICCODE							(0xCC)  //length = 17 byte
#define PICCODE_LEN						(18)
#define EEP_DATA_SIZE					(0xE1)

/**************************************************MSA Page6 Address Define***************************/

/***********Lut Tab Low Temp**************/

#define LUT_LT_TEMP_MSB			(0x80)
#define LUT_LT_TEMP_LSB			(0x81)

#define LUT_LT_VCC_MSB			(0x82)
#define LUT_LT_VCC_LSB			(0x83)

#define LUT_LT_IBIAS0_MSB		(0x84)
#define LUT_LT_IBIAS0_LSB		(0x85)

#define LUT_LT_RXPOWER0_MSB		(0x86)
#define LUT_LT_RXPOWER0_LSB		(0x87)

#define LUT_LT_TXPOWER0_MSB		(0x88)
#define LUT_LT_TXPOWER0_LSB		(0x89)

#define LUT_LT_BIAS0_MSB		(0x8A)
#define LUT_LT_BIAS0_LSB		(0x8B)

#define LUT_LT_MOD0_MSB			(0x8C)
#define LUT_LT_MOD0_LSB			(0x8D)

#define LUT_LT_CROSSING0_MSB	(0x8E)
#define LUT_LT_CROSSING0_LSB	(0x8F)

#define LUT_LT_IBIAS1_MSB		(0x90)
#define LUT_LT_IBIAS1_LSB		(0x91)

#define LUT_LT_RXPOWER1_MSB		(0x92)
#define LUT_LT_RXPOWER1_LSB		(0x93)

#define LUT_LT_TXPOWER1_MSB		(0x94)
#define LUT_LT_TXPOWER1_LSB		(0x95)

#define LUT_LT_BIAS1_MSB		(0x96)
#define LUT_LT_BIAS1_LSB		(0x97)

#define LUT_LT_MOD1_MSB			(0x98)
#define LUT_LT_MOD1_LSB			(0x99)

#define LUT_LT_CROSSING1_MSB	(0x9A)
#define LUT_LT_CROSSING1_LSB	(0x9B)

#define LUT_LT_IBIAS2_MSB		(0x9C)
#define LUT_LT_IBIAS2_LSB		(0x9D)

#define LUT_LT_RXPOWER2_MSB		(0x9E)
#define LUT_LT_RXPOWER2_LSB		(0x9F)

#define LUT_LT_TXPOWER2_MSB		(0xA0)
#define LUT_LT_TXPOWER2_LSB		(0xA1)

#define LUT_LT_BIAS2_MSB		(0xA2)
#define LUT_LT_BIAS2_LSB		(0xA3)

#define LUT_LT_MOD2_MSB			(0xA4)
#define LUT_LT_MOD2_LSB			(0xA5)

#define LUT_LT_CROSSING2_MSB	(0xA6)
#define LUT_LT_CROSSING2_LSB	(0xA7)

#define LUT_LT_IBIAS3_MSB		(0xA8)
#define LUT_LT_IBIAS3_LSB		(0xA9)

#define LUT_LT_RXPOWER3_MSB		(0xAA)
#define LUT_LT_RXPOWER3_LSB		(0xAB)

#define LUT_LT_TXPOWER3_MSB		(0xAC)
#define LUT_LT_TXPOWER3_LSB		(0xAD)

#define LUT_LT_BIAS3_MSB		(0xAE)
#define LUT_LT_BIAS3_LSB		(0xAF)

#define LUT_LT_MOD3_MSB			(0xB0)
#define LUT_LT_MOD3_LSB			(0xB1)

#define LUT_LT_CROSSING3_MSB	(0xB2)
#define LUT_LT_CROSSING3_LSB	(0xB3)

/***********Lut Tab Middle Temp**************/

#define LUT_MT_TEMP_MSB			(0xB4)
#define LUT_MT_TEMP_LSB			(0xB5)

#define LUT_MT_VCC_MSB			(0xB6)
#define LUT_MT_VCC_LSB			(0xB7)

#define LUT_MT_IBIAS0_MSB		(0xB8)
#define LUT_MT_IBIAS0_LSB		(0xB9)

#define LUT_MT_RXPOWER0_MSB		(0xBA)
#define LUT_MT_RXPOWER0_LSB		(0xBB)

#define LUT_MT_TXPOWER0_MSB		(0xBC)
#define LUT_MT_TXPOWER0_LSB		(0xBD)

#define LUT_MT_BIAS0_MSB		(0xBE)
#define LUT_MT_BIAS0_LSB		(0xBF)

#define LUT_MT_MOD0_MSB			(0xC0)
#define LUT_MT_MOD0_LSB			(0xC1)

#define LUT_MT_CROSSING0_MSB	(0xC2)
#define LUT_MT_CROSSING0_LSB	(0xC3)

#define LUT_MT_IBIAS1_MSB		(0xC4)
#define LUT_MT_IBIAS1_LSB		(0xC5)

#define LUT_MT_RXPOWER1_MSB		(0xC6)
#define LUT_MT_RXPOWER1_LSB		(0xC7)

#define LUT_MT_TXPOWER1_MSB		(0xC8)
#define LUT_MT_TXPOWER1_LSB		(0xC9)

#define LUT_MT_BIAS1_MSB		(0xCA)
#define LUT_MT_BIAS1_LSB		(0xCB)

#define LUT_MT_MOD1_MSB			(0xCC)
#define LUT_MT_MOD1_LSB			(0xCD)

#define LUT_MT_CROSSING1_MSB	(0xCE)
#define LUT_MT_CROSSING1_LSB	(0xCF)

#define LUT_MT_IBIAS2_MSB		(0xD0)
#define LUT_MT_IBIAS2_LSB		(0xD1)

#define LUT_MT_RXPOWER2_MSB		(0xD2)
#define LUT_MT_RXPOWER2_LSB		(0xD3)

#define LUT_MT_TXPOWER2_MSB		(0xD4)
#define LUT_MT_TXPOWER2_LSB		(0xD5)

#define LUT_MT_BIAS2_MSB		(0xD6)
#define LUT_MT_BIAS2_LSB		(0xD7)

#define LUT_MT_MOD2_MSB			(0xD8)
#define LUT_MT_MOD2_LSB			(0xD9)

#define LUT_MT_CROSSING2_MSB	(0xDA)
#define LUT_MT_CROSSING2_LSB	(0xDB)

#define LUT_MT_IBIAS3_MSB		(0xDC)
#define LUT_MT_IBIAS3_LSB		(0xDD)

#define LUT_MT_RXPOWER3_MSB		(0xDE)
#define LUT_MT_RXPOWER3_LSB		(0xDF)

#define LUT_MT_TXPOWER3_MSB		(0xE0)
#define LUT_MT_TXPOWER3_LSB		(0xE1)

#define LUT_MT_BIAS3_MSB		(0xE2)
#define LUT_MT_BIAS3_LSB		(0xE3)

#define LUT_MT_MOD3_MSB			(0xE4)
#define LUT_MT_MOD3_LSB			(0xE5)

#define LUT_MT_CROSSING3_MSB	(0xE6)
#define LUT_MT_CROSSING3_LSB	(0xE7)

/**************************************************MSA Page7 Address Define***************************/

/***********Lut Tab High Temp**************/
#define LUT_HT_TEMP_MSB			(0x80)
#define LUT_HT_TEMP_LSB			(0x81)

#define LUT_HT_VCC_MSB			(0x82)
#define LUT_HT_VCC_LSB			(0x83)

#define LUT_HT_IBIAS0_MSB		(0x84)
#define LUT_HT_IBIAS0_LSB		(0x85)

#define LUT_HT_RXPOWER0_MSB		(0x86)
#define LUT_HT_RXPOWER0_LSB		(0x87)

#define LUT_HT_TXPOWER0_MSB		(0x88)
#define LUT_HT_TXPOWER0_LSB		(0x89)

#define LUT_HT_BIAS0_MSB		(0x8A)
#define LUT_HT_BIAS0_LSB		(0x8B)

#define LUT_HT_MOD0_MSB			(0x8C)
#define LUT_HT_MOD0_LSB			(0x8D)

#define LUT_HT_CROSSING0_MSB	(0x8E)
#define LUT_HT_CROSSING0_LSB	(0x8F)

#define LUT_HT_IBIAS1_MSB		(0x90)
#define LUT_HT_IBIAS1_LSB		(0x91)

#define LUT_HT_RXPOWER1_MSB		(0x92)
#define LUT_HT_RXPOWER1_LSB		(0x93)

#define LUT_HT_TXPOWER1_MSB		(0x94)
#define LUT_HT_TXPOWER1_LSB		(0x95)

#define LUT_HT_BIAS1_MSB		(0x96)
#define LUT_HT_BIAS1_LSB		(0x97)

#define LUT_HT_MOD1_MSB			(0x98)
#define LUT_HT_MOD1_LSB			(0x99)

#define LUT_HT_CROSSING1_MSB	(0x9A)
#define LUT_HT_CROSSING1_LSB	(0x9B)

#define LUT_HT_IBIAS2_MSB		(0x9C)
#define LUT_HT_IBIAS2_LSB		(0x9D)

#define LUT_HT_RXPOWER2_MSB		(0x9E)
#define LUT_HT_RXPOWER2_LSB		(0x9F)

#define LUT_HT_TXPOWER2_MSB		(0xA0)
#define LUT_HT_TXPOWER2_LSB		(0xA1)

#define LUT_HT_BIAS2_MSB		(0xA2)
#define LUT_HT_BIAS2_LSB		(0xA3)

#define LUT_HT_MOD2_MSB			(0xA4)
#define LUT_HT_MOD2_LSB			(0xA5)

#define LUT_HT_CROSSING2_MSB	(0xA6)
#define LUT_HT_CROSSING2_LSB	(0xA7)

#define LUT_HT_IBIAS3_MSB		(0xA8)
#define LUT_HT_IBIAS3_LSB		(0xA9)

#define LUT_HT_RXPOWER3_MSB		(0xAA)
#define LUT_HT_RXPOWER3_LSB		(0xAB)

#define LUT_HT_TXPOWER3_MSB		(0xAC)
#define LUT_HT_TXPOWER3_LSB		(0xAD)

#define LUT_HT_BIAS3_MSB		(0xAE)
#define LUT_HT_BIAS3_LSB		(0xAF)

#define LUT_HT_MOD3_MSB			(0xB0)
#define LUT_HT_MOD3_LSB			(0xB1)

#define LUT_HT_CROSSING3_MSB	(0xB2)
#define LUT_HT_CROSSING3_LSB	(0xB3)

#define TEC_TARGET_MSB			(0xFE)
#define TEC_TARGET_LSB			(0xFF)


/**************************************************MSA Page8 Address Define***************************/
#define RX_LOS_MODE				(0x80)
#define RX_LOSA_THLD_CH0_MSB	(0x81)
#define RX_LOSA_THLD_CH0_LSB	(0x82)
#define RX_LOSD_THLD_CH0_MSB	(0x83)
#define RX_LOSD_THLD_CH0_LSB	(0x84)
#define RX_LOSA_THLD_CH1_MSB	(0x85)
#define RX_LOSA_THLD_CH1_LSB	(0x86)
#define RX_LOSD_THLD_CH1_MSB	(0x87)
#define RX_LOSD_THLD_CH1_LSB	(0x88)
#define RX_LOSA_THLD_CH2_MSB	(0x89)
#define RX_LOSA_THLD_CH2_LSB	(0x8A)
#define RX_LOSD_THLD_CH2_MSB	(0x8B)
#define RX_LOSD_THLD_CH2_LSB	(0x8C)
#define RX_LOSA_THLD_CH3_MSB	(0x8D)
#define RX_LOSA_THLD_CH3_LSB	(0x8E)
#define RX_LOSD_THLD_CH3_MSB	(0x8F)
#define RX_LOSD_THLD_CH3_LSB	(0x90)
#define RSSI_DRAKA_THLD_CH0_MSB	(0x91)
#define RSSI_DRAKA_THLD_CH0_LSB	(0x92)
#define RSSI_DRAKD_THLD_CH0_MSB	(0x93)
#define RSSI_DRAKD_THLD_CH0_LSB	(0x94)
#define RSSI_DRAKA_THLD_CH1_MSB	(0x95)
#define RSSI_DRAKA_THLD_CH1_LSB	(0x96)
#define RSSI_DRAKD_THLD_CH1_MSB	(0x97)
#define RSSI_DRAKD_THLD_CH1_LSB	(0x98)
#define RSSI_DRAKA_THLD_CH2_MSB	(0x99)
#define RSSI_DRAKA_THLD_CH2_LSB	(0x9A)
#define RSSI_DRAKD_THLD_CH2_MSB	(0x9B)
#define RSSI_DRAKD_THLD_CH2_LSB	(0x9C)
#define RSSI_DRAKA_THLD_CH3_MSB	(0x9D)
#define RSSI_DRAKA_THLD_CH3_LSB	(0x9E)
#define RSSI_DRAKD_THLD_CH3_MSB	(0x9F)
#define RSSI_DRAKD_THLD_CH3_LSB	(0xA0)


/***********Ddmi cali para save**************/
//MSA page6

#define PAGE6_DDMI_CALI_TEMP_OFFSET_MSB			(0xE8)
#define PAGE6_DDMI_CALI_TEMP_OFFSET_LSB			(0xE9)

#define PAGE6_DDMI_CALI_VCC_OFFSET_MSB			(0xEA)
#define PAGE6_DDMI_CALI_VCC_OFFSET_LSB			(0xEB)

#define PAGE6_DDMI_CALI_CH0_BIAS_BASE_ADDR  	(0xEC)
#define PAGE6_DDMI_CALI_CH0_TXPOWER_BASE_ADDR  	(0xF8)
#define PAGE6_DDMI_CALI_CH0_RXPOWER_BASE_ADDR  	(0xF0)

//MSA page7
#define PAGE7_DDMI_CALI_CH1_BIAS_BASE_ADDR  	(0xB4)
#define PAGE7_DDMI_CALI_CH1_TXPOWER_BASE_ADDR  	(0xC0)
#define PAGE7_DDMI_CALI_CH1_RXPOWER_BASE_ADDR  	(0xB8)

#define PAGE7_DDMI_CALI_CH2_BIAS_BASE_ADDR  	(0xC8)
#define PAGE7_DDMI_CALI_CH2_TXPOWER_BASE_ADDR  	(0xD4)
#define PAGE7_DDMI_CALI_CH2_RXPOWER_BASE_ADDR  	(0xCC)

#define PAGE7_DDMI_CALI_CH3_BIAS_BASE_ADDR  	(0xDC)
#define PAGE7_DDMI_CALI_CH3_TXPOWER_BASE_ADDR  	(0xE8)
#define PAGE7_DDMI_CALI_CH3_RXPOWER_BASE_ADDR  	(0xE0)

#define PAGE6_LT_CH0_LASER_CFG_BASE_ADDR (0x8A)
#define PAGE6_LT_CH1_LASER_CFG_BASE_ADDR (0x96)
#define PAGE6_LT_CH2_LASER_CFG_BASE_ADDR (0xA2)
#define PAGE6_LT_CH3_LASER_CFG_BASE_ADDR (0xAE)

#define PAGE6_MT_CH0_LASER_CFG_BASE_ADDR (0xBE)
#define PAGE6_MT_CH1_LASER_CFG_BASE_ADDR (0xCA)
#define PAGE6_MT_CH2_LASER_CFG_BASE_ADDR (0xD6)
#define PAGE6_MT_CH3_LASER_CFG_BASE_ADDR (0xE2)

#define PAGE7_HT_CH0_LASER_CFG_BASE_ADDR (0x8A)
#define PAGE7_HT_CH1_LASER_CFG_BASE_ADDR (0x96)
#define PAGE7_HT_CH2_LASER_CFG_BASE_ADDR (0xA2)
#define PAGE7_HT_CH3_LASER_CFG_BASE_ADDR (0xAE)

/*
#define DDMI_CALI_CH0_BIAS1_OFFSET_MSB		(0xEC)
#define DDMI_CALI_CH0_BIAS1_OFFSET_LSB		(0xED)

#define DDMI_CALI_CH0_BIAS2_OFFSET_MSB		(0xEE)
#define DDMI_CALI_CH0_BIAS2_OFFSET_LSB		(0xEF)

#define DDMI_CALI_CH0_RXPOWR1_OFFSET_MSB	(0xF0)
#define DDMI_CALI_CH0_RXPOWR1_OFFSET_LSB	(0xF1)

#define DDMI_CALI_CH0_RSSI1_OFFSET_MSB		(0xF2)
#define DDMI_CALI_CH0_RSSI1_OFFSET_LSB		(0xF3)

#define DDMI_CALI_CH0_RXPOWR2_OFFSET_MSB	(0xF4)
#define DDMI_CALI_CH0_RXPOWR2_OFFSET_LSB	(0xF5)

#define DDMI_CALI_CH0_RSSI2_OFFSET_MSB		(0xF6)
#define DDMI_CALI_CH0_RSSI2_OFFSET_LSB		(0xF7)

#define DDMI_CALI_CH0_TXPOWR1_OFFSET_MSB	(0xF8)
#define DDMI_CALI_CH0_TXPOWR1_OFFSET_LSB	(0xF9)

#define DDMI_CALI_CH0_TXMON1_OFFSET_MSB		(0xFA)
#define DDMI_CALI_CH0_TXMON1_OFFSET_LSB		(0xFB)

#define DDMI_CALI_CH0_TXPOWR2_OFFSET_MSB	(0xFC)
#define DDMI_CALI_CH0_TXPOWR2_OFFSET_LSB	(0xFD)

#define DDMI_CALI_CH0_TXMON2_OFFSET_MSB		(0xFE)
#define DDMI_CALI_CH0_TXMON2_OFFSET_LSB		(0xFF)

*/

//MSA page7

/**************************************************MSA Page10 Address Define***************************/
#define PAGE10_START_ADDR 		(0x80)

typedef enum
{
	DDMI_CALI_BEFORE  = 0x0,
	DDMI_CALI_AFTER   = 0x1,
}DDMI_CALI_TYPE;

typedef enum
{
	TXCDR_TX 			= 0x0,
	TXCDR_BISA_DAC  	= 0x1,  //SR4 101, LR4 301, CWDM4-206,CWDM4-202
	TXCDR_IBIAS			= 0x2,
	TXCDR_MOD_DAC  		= 0x3,  //SR4 101, LR4 301, CWDM4-206,CWDM4-202
	TXCDR_IMOD			= 0x4,
	TXCDR_CROSS_DAC  	= 0x5,  //SR4 101, LR4 301, CWDM4-206,CWDM4-202
	TXCDR_CROSS		  	= 0x6,
	TXCDR_SLEW			= 0x7,
	TXCDR_PREFALL		= 0x8,
	TXCDR_TDE			= 0x9,
	TXCDR_LASER			= 0xA,
	RXCDR_DEEM			= 0xB,
	RXCDR_SLA_EN		= 0xC,
	RXCDR_AGC			= 0xD,
	RXCDR_SLICE			= 0xE,
	RXCDR_RSSI_ADC		= 0xF,
	TXCDR_EQ			= 0x10,//SR 101, LR4 301
	POWER_ADJUST		= 0x11, //CWDM4-202
	TXCDR_TEMP			= 0x12, //CWDM4-202
	TXCDR_VCC			= 0x13, //CWDM4-202
	TXCDR_HFDE			= 0x14, //CWDM4-202
	TXCDR_LFDE			= 0x15, //CWDM4-202
	TXCDR_ASY			= 0x16, //CWDM4-202
	TXCDR_IBIASMOD		= 0x17,
}CDR_REG;

typedef enum
{
	DBG_READ_CMD            = 1,
	DBG_WRITE_CMD           = 2,
	DBG_DEBUG_LOG_CMD       = 4,
	DBG_AUTO_ENABLE_CMD		= 5,
	DBG_AUTO_DISABLE_CMD	= 6,
	DBG_APC_CALI_CMD        = 7,
	DBG_JUMP_TO_BOOT_CMD	= 9,

	DBG_DDMI_ENABLE_CMD		= 0xa,
	DBG_DDMI_DISABLE_CMD	= 0xb,
	DBG_RESET_CMD			= 0xc,
	DBG_EEP_INIT_CMD		= 0xd,
	DBG_SAVE_REG_TO_EEP_CMD	= 0xe,
	DBG_SET_CUR_LUT_CMD		= 0xf,
	DBG_SET_MOD_CONSOLE		= 0x10,
	DBG_TEC_ENABLE_CMD		= 0x11,
	DBG_LASER_ON_CMD		= 0x12,
	DBG_LASER_OFF_CMD		= 0x13,
	DBG_FACTORY_RESET_CMD	= 0x14,
	DBG_AGING_START_CMD		= 0x15,
	DBG_AGING_STOP_CMD		= 0x16,
	DBG_AGING_EEP_INIT_CMD	= 0x17,
	DBG_BONDING_MODE		= 0x18,
	DBG_I2C_TXCDR_ENABLE	= 0x19,
	DBG_I2C_RXCDR_ENABLE	= 0x1A,
	DBG_EEP_ERASE_CMD		= 0x1B,
	DBG_UNKNOW_CMD			= 0xFF,
}MSA_PAGE4_DBG_CMD_T;

typedef enum
{
	FSR_TEST_BOARD_TYPE_LR4        = 0,
	FSR_TEST_BOARD_TYPE_AOC_M      = 1,
	FSR_TEST_BOARD_TYPE_AOC_S      = 2,
	FSR_TEST_BOARD_TYPE_CWDM4_202  = 3,
	FSR_TEST_BOARD_UNKNOW_TYPE	   	  ,
}TEST_BOARD_WORK_TYPE;

typedef enum
{
	DISABLE_LOG_CMD	= 0x0,
	ENABLE_LOG_CMD	= 0x1,
	READ_LOG_CMD	= 0x2,
	CLEAR_LOG_CMD	= 0xc,
}DEBUG_LOG_ACTION;

typedef enum
{
	SAVE_CUR_LT_LUT_TAB  = 0x1,
	SAVE_CUR_MT_LUT_TAB  = 0x2,
	SAVE_CUR_HT_LUT_TAB  = 0x3,
}SAVE_CUR_LUT_TAB;

typedef enum
{
	DDMI_VCC 	 = 0x0,
	DDMI_TEMP	 = 0x1,
	DDMI_BIAS	 = 0x2,
	DDMI_TXMON	 = 0x3,
	DDMI_RSSI	 = 0x4,
}DDMI_DATA_TYPE;

typedef enum
{
  MODULE_TYPE_SR4		= 0x1,
  MODULE_TYPE_CWDM4		= 0x2,
  MODULE_TYPE_LR4		= 0x3,
}MODULE_TYPE;

typedef enum
{
	EEP_INIT  = 0x1,
	EEP_SAVE  = 0x2,
	EEP_SHOW  = 0x3,
	EEP_ERASE = 0x4,
	EEP_UNKNOW    ,
}EEP_ACTION;

typedef struct
{
	uint8_t  devAddr;
	uint8_t  value;
	uint16_t offsetAddr;
}SAVE_REG_T;

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
	uint8_t ver_num_msb;
	uint8_t ver_num_lsb;
}MODULE_REALEASE_INFO;

typedef struct
{
	uint8_t msb;
	uint8_t lsb;
}DATA_UNIT;


typedef struct {
	int16_t 		temp;
	uint16_t		bias;
	uint16_t		mod;
	int16_t			cross;
}MSA_LASER_CH_CFG_T;

typedef struct {
	uint16_t		bias;
	uint16_t		mod;
	int16_t			cross;
}LASER_CFG_T;

typedef enum {
	MSA_LUT_TYPE_LOW_TEMP	= 0,
	MSA_LUT_TYPE_MID_TEMP	= 1,
	MSA_LUT_TYPE_HI_TEMP	= 2,
	MSA_LUT_TYPE_MAX		= 3,
}MSA_LUT_TYPE;


typedef enum
{
	CHANNEL0  		= 0,
	CHANNEL1  		= 1,
	CHANNEL2  		= 2,
	CHANNEL3  		= 3,
	CHANNEL_MAX  	= 4,
}CHANNEL_NUM;

typedef enum
{
	ACTION_ENABLE 	  = 0,
	ACTION_DISABLE	  = 1,
	ACTION_I2C_READ   = 2,
	ACTION_I2C_WRITE  = 3,
}I2C_ACTION;


typedef enum {
	MSA_PAGE_LOWER	= 0xfe,
	MSA_PAGE_00H	= 0x0,
	MSA_PAGE_01H	= 0x1,
	MSA_PAGE_02H	= 0x2,
	MSA_PAGE_03H	= 0x3,
	SFP_PAGE_04H	= 0x4,
	SFP_PAGE_05H    = 0x5,
	SFP_PAGE_06H    = 0x6,
	SFP_PAGE_07H    = 0x7,
	SFP_PAGE_08H    = 0x8,
	SFP_PAGE_09H    = 0x9,
	SFP_PAGE_0AH    = 0xa,
	SFP_NONE_PAGE	= 0xb,
}SFP_PAGE;

/*MSA Lower Page Define*/
typedef struct
{
	int16_t  temp;  	//byte 22, 23
	uint8_t  resv0[2];  //byte 24, 25
	uint16_t vcc;		//byte 26, 27
	uint8_t  resv1[6];  //byte 28, 29, 30, 31, 32, 33
	uint16_t rssi[MAX_CHANNEL_NUMS];  	 //byte 34,35,36,37,38,39,40,41
	uint16_t txbias[MAX_CHANNEL_NUMS];	 //byte 42,43,44,45,46,47,48,49
	uint16_t txpower[MAX_CHANNEL_NUMS];  //byte 50,51,52,53,54,55,56,57
}MSA_PAGE_LOWER_MON_T;

#define MSA_PAGE_LOWER_MON_BASE_ADDR	(22)

/*Ddmi cali parameter*/
typedef struct {
	uint16_t txpower1;
	uint16_t bias1;
	uint16_t txpower2;
	uint16_t bias2;
}TXPOWER_CALI_PARA_T;

typedef struct {
	uint16_t rxpower1;
	uint16_t rssi1;
	uint16_t rxpower2;
	uint16_t rssi2;
}RXPOWER_CALI_PARA_T;

typedef struct{
	uint16_t bias_offset1;  // in 0.1mA
	uint16_t bias_offset2;
}BIAS_CALI_PARA_T;

typedef struct {
	BIAS_CALI_PARA_T	bias;
	RXPOWER_CALI_PARA_T	rxpower;
	TXPOWER_CALI_PARA_T	txpower;
}CURRENT_CALI_PARA_T;

typedef struct {
	int16_t temp;	// in 1/256 C
	int16_t vcc;	// in 0.1mV
	CURRENT_CALI_PARA_T current[MAX_CHANNEL_NUMS];
}DDMI_CALI_PARA_T;

typedef enum {
	TEMP_HIGH_ALARM		= 0x0,
	TEMP_LOW_ALARM		= 0x1,
	TEMP_HIGH_WARN		= 0x2,
	TEMP_LOW_WARN  	    = 0x3,
	VCC_HIGH_ALARM      = 0x4,
	VCC_LOW_ALARM	    = 0x5,
	VCC_HIGH_WARN	    = 0x6,
	VCC_LOW_WARN	    = 0x7,
	RXPWR_HIGH_ALARM	= 0x8,
	RXPWR_LOW_ALARM	    = 0x9,
	RXPWR_HIGH_WARN 	= 0xa,
	RXPWR_LOW_WARN      = 0xb,
	TXBIAS_HIGH_ALARM	= 0xc,
	TXBIAS_LOW_ALARM	= 0xd,
	TXBIAS_HIGH_WARN	= 0xe,
	TXBIAS_LOW_WARN     = 0xf,
	TXPWR_HIGH_ALARM	= 0x10,
	TXPWR_LOW_ALARM	    = 0x11,
	TXPWR_HIGH_WARN	    = 0x12,
	TXPWR_LOW_WARN      = 0x13,
	HTRHLD_ALL   		= 0x14,
	UNKNOW_ALARM		= 0xFF,
}MSA_PAGE3_ALARM_WARING_THRESHOLD_T;

typedef enum
{
	PART_LUT_INFO = 0x0,
	ALL_LUT_INFO  = 0x1,
}LUT_SHOW_TYPE;

typedef struct
{
	uint8_t ddmi_enable;
	uint8_t auto_enable;
}POLL_T;


void     com_delay(uint16_t i);
void 	 com_delay_us(uint8_t us);
int16_t  com_str_to_int16(uint8_t *str);
bool 	 com_get_module_boot_status();
uint8_t _qsfp28_get_channel(const char  *s);
uint16_t com_average_value16(uint16_t *buffer, uint8_t length);
uint8_t	qsfp28_get_mod_release_info();
uint32_t qsfp28_get_mod_soft_type();
uint8_t qsfp28_get_mod_type();
uint8_t qsfp28_get_ddmi(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_TYPE type);
uint8_t qsfp28_get_cur_apc_value(uint8_t channel);
uint8_t qsfp28_get_fixed_temp_point_apc_value(int8_t temp, uint8_t channel);
uint8_t	qsfp28_set_lut_tab_para_from_reg_inner(uint8_t channel, SAVE_CUR_LUT_TAB cur_lut);
uint8_t qsfp28_dbg_i2c_write_byte(uint16_t offset_addr,uint8_t value);
uint8_t qsfp28_set_cur_apc_value(uint8_t channel);
uint8_t qsfp28_set_fixed_temp_point_apc_value(int8_t temp, uint8_t channel);
uint8_t qsfp28_dbg_i2c_read(uint8_t dev_addr, uint16_t offset_addr, uint8_t *value, uint8_t len);
uint8_t qsfp28_dbg_i2c_write(uint8_t dev_addr, uint8_t page, uint16_t offset_addr, uint8_t *value, uint8_t len);
uint8_t qsfp28_debug_log_handle(DEBUG_LOG_ACTION action);
uint8_t	qsfp28_eep_handle(EEP_ACTION action, uint8_t dev_addr, uint16_t offset_addr, uint8_t value);
uint8_t qsfp28_get_msa_lower_page_monitor();
uint8_t qsfp28_set_ddmi_cali_para(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_PARA_T* cali_para);
uint8_t qsfp28_get_ddmi_cali_para(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_PARA_T* cali_para);
uint8_t qsfp28_get_tec(SFP_PAGE page,uint8_t tec,uint16_t *Pdata);
uint8_t qsfp28_set_tec(SFP_PAGE page,uint8_t tec,uint16_t *Pdata);
uint8_t qsfp28_config_mod_console(bool enable);
uint8_t qsfp28_set_thrhld_value(MSA_PAGE3_ALARM_WARING_THRESHOLD_T type,uint16_t value);
uint8_t qsfp28_get_thrhld_value(MSA_PAGE3_ALARM_WARING_THRESHOLD_T type);
uint8_t qsfp28_msa_page4_dbg_cmd(MSA_PAGE4_DBG_CMD_T cmd);
uint8_t qsfp28_set_lut_tab(uint8_t channel, MSA_LUT_TYPE type,  MSA_LASER_CH_CFG_T cfg);
uint8_t qsfp28_get_lut_tab(uint8_t channel,LUT_SHOW_TYPE show_type, MSA_LUT_TYPE type);
uint8_t qsfp28_get_module_class();
uint8_t qsfp28_set_module_class(uint8_t value);
uint8_t qsfp28_common_rssi_show();
bool qsfp28_get_cmd_status();
uint8_t qsfp28_enable_module_poll();
uint8_t qsfp28_disable_module_poll();
uint8_t qsfp28_select_page(uint8_t BaseAddr,uint16_t OffsetAddr,uint8_t page);
uint8_t   smb_write_page_byte(uint8_t BaseAddr, uint8_t page, uint16_t OffsetAddr,uint8_t *Data, uint16_t Length);
uint8_t   smb_read_page_byte(uint8_t BaseAddr, uint8_t page, uint16_t OffsetAddr,uint8_t *Data, uint16_t Length);

#endif
