/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* https://www.renesas.com/disclaimer
*
* Copyright (C) 2024 Renesas Electronics Corporation.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_touch_config.c
* Description  : This file includes module implementations.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History      : MM/DD/YYYY Version Description
*              : 06/23/2020 1.00    First Release
*              : 07/15/2021 1.10    Adding Diagnosis Supporting
*              : 11/13/2021 1.20    Adding information for Initial Offset Tuning
*              : 03/16/2022 1.30    Adding Multiple Electrode Connection Supporting
*              : 03/31/2023 1.31    Improving Traceability
*              : 04/18/2023 1.40    Adding 3 Frequency Judgement Supporting
*              : 05/27/2024 1.50    Adding Auto Judgement Supporting
***********************************************************************************************************************/
/***********************************************************************************************************************
* Touch I/F Configuration File  : G23_RSSK_Slider_QE_Log_250430.tifcfg
* Tuning Log File               : G23_RSSK_Slider_QE_Log_250430_log_tuning20250501003656.log
***********************************************************************************************************************/

#include "qe_touch_config.h"

volatile uint8_t      g_qe_touch_flag;
volatile ctsu_event_t g_qe_ctsu_event;

void qe_touch_callback(touch_callback_args_t * p_args)
{
    g_qe_touch_flag = 1;
    g_qe_ctsu_event = p_args -> event;
}













/***********************************************************************************************************************
CTSU Related Information for [CONFIG01] configuration.
***********************************************************************************************************************/

const ctsu_element_cfg_t g_qe_ctsu_element_cfg_config01[] =
{
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x106, .snum = 0x07, .sdpa = 0x05 },
};

const ctsu_cfg_t g_qe_ctsu_cfg_config01 =
{
    .cap = CTSU_CAP_SOFTWARE,

    .txvsel = CTSU_TXVSEL_VCC,
    .txvsel2= CTSU_TXVSEL_MODE,

    .atune12= CTSU_ATUNE12_40UA,
    .md     = CTSU_MODE_SELF_MULTI_SCAN,
    .posel  = CTSU_POSEL_LOW_GPIO,


    .ctsuchac0  = 0x20, /* ch0-ch7 enable mask */
    .ctsuchac1  = 0x00, /* ch8-ch15 enable mask */
    .ctsuchac2  = 0x00, /* ch16-ch23 enable mask */
    .ctsuchac3  = 0x00, /* ch24-ch31 enable mask */
    .ctsuchac4  = 0x00, /* ch32-ch39 enable mask */
    .ctsuchtrc0 = 0x00, /* ch0-ch7 mutual tx mask */
    .ctsuchtrc1 = 0x00, /* ch8-ch15 mutual tx mask */
    .ctsuchtrc2 = 0x00, /* ch16-ch23 mutual tx mask */
    .ctsuchtrc3 = 0x00, /* ch24-ch31 mutual tx mask */
    .ctsuchtrc4 = 0x00, /* ch32-ch39 mutual tx mask */
    .num_rx     = 1,
    .num_tx     = 0,
    .p_elements = g_qe_ctsu_element_cfg_config01,

#if (CTSU_TARGET_VALUE_CONFIG_SUPPORT == 1)
    .tuning_self_target_value   = 11520,
    .tuning_mutual_target_value = 10240,
#endif

    .num_moving_average = 4,
    .tunning_enable     = true,
    .p_callback    = &qe_touch_callback,

};

ctsu_instance_ctrl_t g_qe_ctsu_ctrl_config01;

const ctsu_instance_t g_qe_ctsu_instance_config01 =
{
    .p_ctrl = &g_qe_ctsu_ctrl_config01,
    .p_cfg  = &g_qe_ctsu_cfg_config01,
    .p_api  = &g_ctsu_on_ctsu,
};

/***********************************************************************************************************************
Touch Related Information for [CONFIG01] configuration.
***********************************************************************************************************************/

#define QE_TOUCH_CONFIG01_NUM_BUTTONS (1)
#define QE_TOUCH_CONFIG01_NUM_SLIDERS (0)
#define QE_TOUCH_CONFIG01_NUM_WHEELS  (0)

/* Button configurations */
#if (QE_TOUCH_CONFIG01_NUM_BUTTONS != 0)
const touch_button_cfg_t g_qe_touch_button_cfg_config01[] =
{

    /* VMM */
    /* button00 */
    {
        .elem_index = 0,
        .threshold  = 1450,
        .hysteresis = 72,
    },
};
#endif

/* Slider configurations */
#if (QE_TOUCH_CONFIG01_NUM_SLIDERS != 0)
const touch_slider_cfg_t g_qe_touch_slider_cfg_config01[] =
{
    NULL
};
#endif

/* Wheel configurations */
#if (QE_TOUCH_CONFIG01_NUM_WHEELS != 0)
const touch_wheel_cfg_t g_qe_touch_wheel_cfg_config01[] =
{
    NULL
};
#endif

/* Touch configurations */
const touch_cfg_t g_qe_touch_cfg_config01 =
{
    .p_buttons   = g_qe_touch_button_cfg_config01,
    .p_sliders   = NULL,
    .p_wheels    = NULL,
    .num_buttons = QE_TOUCH_CONFIG01_NUM_BUTTONS,
    .num_sliders = QE_TOUCH_CONFIG01_NUM_SLIDERS,
    .num_wheels  = QE_TOUCH_CONFIG01_NUM_WHEELS,

    .number        = 0,

    .on_freq     = 3,
    .off_freq    = 3,
    .drift_freq  = 255,
    .cancel_freq = 0,

    .p_ctsu_instance = &g_qe_ctsu_instance_config01,
};

touch_instance_ctrl_t g_qe_touch_ctrl_config01;

const touch_instance_t g_qe_touch_instance_config01 =
{
    .p_ctrl = &g_qe_touch_ctrl_config01,
    .p_cfg  = &g_qe_touch_cfg_config01,
    .p_api  = &g_touch_on_ctsu,
};

/***********************************************************************************************************************
CTSU Related Information for [CONFIG02] configuration.
***********************************************************************************************************************/

const ctsu_element_cfg_t g_qe_ctsu_element_cfg_config02[] =
{
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x12B, .snum = 0x07, .sdpa = 0x05 },
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x112, .snum = 0x07, .sdpa = 0x05 },
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x10E, .snum = 0x07, .sdpa = 0x05 },
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x0F9, .snum = 0x07, .sdpa = 0x05 },
    { .ssdiv = CTSU_SSDIV_4000, .so = 0x0E4, .snum = 0x07, .sdpa = 0x05 },
};

const ctsu_cfg_t g_qe_ctsu_cfg_config02 =
{
    .cap = CTSU_CAP_SOFTWARE,

    .txvsel = CTSU_TXVSEL_VCC,
    .txvsel2= CTSU_TXVSEL_MODE,

    .atune12= CTSU_ATUNE12_40UA,
    .md     = CTSU_MODE_SELF_MULTI_SCAN,
    .posel  = CTSU_POSEL_LOW_GPIO,


    .ctsuchac0  = 0x02, /* ch0-ch7 enable mask */
    .ctsuchac1  = 0xF0, /* ch8-ch15 enable mask */
    .ctsuchac2  = 0x00, /* ch16-ch23 enable mask */
    .ctsuchac3  = 0x00, /* ch24-ch31 enable mask */
    .ctsuchac4  = 0x00, /* ch32-ch39 enable mask */
    .ctsuchtrc0 = 0x00, /* ch0-ch7 mutual tx mask */
    .ctsuchtrc1 = 0x00, /* ch8-ch15 mutual tx mask */
    .ctsuchtrc2 = 0x00, /* ch16-ch23 mutual tx mask */
    .ctsuchtrc3 = 0x00, /* ch24-ch31 mutual tx mask */
    .ctsuchtrc4 = 0x00, /* ch32-ch39 mutual tx mask */
    .num_rx     = 5,
    .num_tx     = 0,
    .p_elements = g_qe_ctsu_element_cfg_config02,

#if (CTSU_TARGET_VALUE_CONFIG_SUPPORT == 1)
    .tuning_self_target_value   = 11520,
    .tuning_mutual_target_value = 10240,
#endif

    .num_moving_average = 4,
    .tunning_enable     = true,
    .p_callback    = &qe_touch_callback,

};

ctsu_instance_ctrl_t g_qe_ctsu_ctrl_config02;

const ctsu_instance_t g_qe_ctsu_instance_config02 =
{
    .p_ctrl = &g_qe_ctsu_ctrl_config02,
    .p_cfg  = &g_qe_ctsu_cfg_config02,
    .p_api  = &g_ctsu_on_ctsu,
};

/***********************************************************************************************************************
Touch Related Information for [CONFIG02] configuration.
***********************************************************************************************************************/

#define QE_TOUCH_CONFIG02_NUM_BUTTONS (0)
#define QE_TOUCH_CONFIG02_NUM_SLIDERS (1)
#define QE_TOUCH_CONFIG02_NUM_WHEELS  (0)

/* Button configurations */
#if (QE_TOUCH_CONFIG02_NUM_BUTTONS != 0)
const touch_button_cfg_t g_qe_touch_button_cfg_config02[] =
{

    /* VMM */
    NULL
};
#endif

/* Slider configurations */
const uint8_t g_qe_touch_elem_slider_config02_slider00[] =
{
    4, 3, 2, 1, 0
};

#if (QE_TOUCH_CONFIG02_NUM_SLIDERS != 0)
const touch_slider_cfg_t g_qe_touch_slider_cfg_config02[] =
{
    /* slider00 */
    {
        .p_elem_index = g_qe_touch_elem_slider_config02_slider00,
        .num_elements = 5,
        .threshold    = 1807,
    },
};
#endif

/* Wheel configurations */
#if (QE_TOUCH_CONFIG02_NUM_WHEELS != 0)
const touch_wheel_cfg_t g_qe_touch_wheel_cfg_config02[] =
{
    NULL
};
#endif

/* Touch configurations */
const touch_cfg_t g_qe_touch_cfg_config02 =
{
    .p_buttons   = NULL,
    .p_sliders   = g_qe_touch_slider_cfg_config02,
    .p_wheels    = NULL,
    .num_buttons = QE_TOUCH_CONFIG02_NUM_BUTTONS,
    .num_sliders = QE_TOUCH_CONFIG02_NUM_SLIDERS,
    .num_wheels  = QE_TOUCH_CONFIG02_NUM_WHEELS,

    .number        = 1,

    .on_freq     = 3,
    .off_freq    = 3,
    .drift_freq  = 255,
    .cancel_freq = 0,

    .p_ctsu_instance = &g_qe_ctsu_instance_config02,
};

touch_instance_ctrl_t g_qe_touch_ctrl_config02;

const touch_instance_t g_qe_touch_instance_config02 =
{
    .p_ctrl = &g_qe_touch_ctrl_config02,
    .p_cfg  = &g_qe_touch_cfg_config02,
    .p_api  = &g_touch_on_ctsu,
};

