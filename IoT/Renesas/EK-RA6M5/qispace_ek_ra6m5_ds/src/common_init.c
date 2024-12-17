/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/
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
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_init.h"
#include "board_cfg.h"



#define NUM_RATES             (sizeof(pwm_rates) / sizeof(pwm_rates[0]))
#define NUM_DCS               (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))
#define NUM_SWITCH            (sizeof(irq_pins) / sizeof(irq_pins[0]))

uint32_t g_curr_led_freq = BLINK_FREQ_1HZ;

static const struct
{
    const external_irq_instance_t * const p_irq;
} irq_pins[] =
{
        {&g_external_irq10},
        {&g_external_irq9},
};

static const struct
{
    const timer_instance_t * const p_timer;
    const gpt_io_pin_t              pin;
} pwm_pins[] =
{
    {&g_gpt_red, GPT_IO_PIN_GTIOCA},
    {&g_gpt_green, GPT_IO_PIN_GTIOCA},
    {&g_gpt_blue, GPT_IO_PIN_GTIOCA},
};


static uint32_t cur_dc = 0, cur_rate = 0;

int curr_led_freq = 0;
uint32_t pwm_dcs[3] = {LED_INTENSITY_10, LED_INTENSITY_50, LED_INTENSITY_90};
uint32_t pwm_rates[3] = {BLINK_FREQ_1HZ, BLINK_FREQ_5HZ, BLINK_FREQ_10HZ};


void button_irq9_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if(++cur_rate > (NUM_RATES - 1))
        cur_rate = 0;

    R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);

    g_curr_led_freq = (uint32_t)pwm_rates[cur_rate];
}

void button_irq10_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    if(++cur_dc > (NUM_DCS - 1))
        cur_dc = 0;

    for(int j = 0; j < g_bsp_leds.led_count; j++)
    {
        R_GPT_DutyCycleSet(pwm_pins[j].p_timer->p_ctrl, pwm_dcs[cur_dc], pwm_pins[j].pin);
    }
}

static fsp_err_t ICU_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for(uint32_t i = 0; i < NUM_SWITCH; i++)
    {
        fsp_err = R_ICU_ExternalIrqOpen(irq_pins[i].p_irq->p_ctrl, irq_pins[i].p_irq->p_cfg);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;

        fsp_err = R_ICU_ExternalIrqEnable(irq_pins[i].p_irq->p_ctrl);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    return fsp_err;
}

void gpt_blink_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(pwm_pins[0].p_timer->p_ctrl, &status);

        for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
        {
            if (TIMER_STATE_COUNTING == status.state)
            {
                R_GPT_Stop(pwm_pins[i].p_timer->p_ctrl);
            }
            else
            {
                R_GPT_Start(pwm_pins[i].p_timer->p_ctrl);
            }
        }
    }
}

static fsp_err_t GPT_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for(uint32_t i = 0; i < g_bsp_leds.led_count; i++)
    {
        fsp_err = R_GPT_Open(pwm_pins[i].p_timer->p_ctrl, pwm_pins[i].p_timer->p_cfg);
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }
    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, pwm_rates[cur_rate]);
    {
        if(FSP_SUCCESS != fsp_err)
            return fsp_err;
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);
    if(FSP_SUCCESS != fsp_err)
    {
        /* Turn ON RED LED to indicate fatal error */
        TURN_RED_ON;

        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}

static fsp_err_t ADC_Initialize(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = R_ADC_ScanStart(&g_adc_ctrl);
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}

fsp_err_t common_init(void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    fsp_err = GPT_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ICU_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    fsp_err = ADC_Initialize();
    if(FSP_SUCCESS != fsp_err)
        return fsp_err;

    return fsp_err;
}
