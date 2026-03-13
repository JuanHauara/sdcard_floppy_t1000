#include "toshiba_fdd_iface.h"

#include <string.h>

#include "board_gpio.h"
#include "debug_log.h"
#include "utils.h"


#define TOSHIBA_FDD_POLL_INTERVAL_US    (250u)


static void toshiba_fdd_iface_configure_signal(toshiba_fdd_signal_level_t *signal,
                                               bool is_configured,
                                               bool level)
{
    if (signal == NULL)
    {
        return;
    }

    signal->is_configured = is_configured;
    signal->level = level;
}

static void toshiba_fdd_iface_decode_signal(bool raw_level,
                                            const toshiba_fdd_signal_level_t *signal,
                                            bool *out_value,
                                            bool *out_valid)
{
    if ((signal == NULL) || (out_value == NULL) || (out_valid == NULL))
    {
        return;
    }

    if (!signal->is_configured)
    {
        *out_value = false;
        *out_valid = false;
        return;
    }

    *out_value = (raw_level == signal->level);
    *out_valid = true;
}

static bool toshiba_fdd_iface_read_output_raw(board_gpio_do_pin_t pin)
{
    return board_gpio_read_do(pin);
}

static void toshiba_fdd_iface_read_outputs_raw(toshiba_fdd_outputs_t *outputs)
{
    if (outputs == NULL)
    {
        return;
    }

    outputs->index_raw = toshiba_fdd_iface_read_output_raw(FDD_INDEX);
    outputs->dskchg_raw = toshiba_fdd_iface_read_output_raw(FDD_DSKCHG);
    outputs->ready_raw = toshiba_fdd_iface_read_output_raw(FDD_READY);
    outputs->media_raw = toshiba_fdd_iface_read_output_raw(FDD_MEDIA);
    outputs->track0_raw = toshiba_fdd_iface_read_output_raw(FDD_TRACK0);
    outputs->wprotc_raw = toshiba_fdd_iface_read_output_raw(FDD_WPROTC);
    outputs->rdda_raw = toshiba_fdd_iface_read_output_raw(FDD_RDDA);
}

static void toshiba_fdd_iface_apply_signal(bool logical_value,
                                           bool logical_valid,
                                           const toshiba_fdd_signal_level_t *signal,
                                           bool *raw_level)
{
    if ((signal == NULL) || (raw_level == NULL))
    {
        return;
    }

    if (!logical_valid || !signal->is_configured)
    {
        return;
    }

    *raw_level = (logical_value == signal->level);
}

static void toshiba_fdd_iface_write_outputs_raw(const toshiba_fdd_outputs_t *outputs)
{
    if (outputs == NULL)
    {
        return;
    }

    board_gpio_write_do(FDD_INDEX, outputs->index_raw);
    board_gpio_write_do(FDD_DSKCHG, outputs->dskchg_raw);
    board_gpio_write_do(FDD_READY, outputs->ready_raw);
    board_gpio_write_do(FDD_MEDIA, outputs->media_raw);
    board_gpio_write_do(FDD_TRACK0, outputs->track0_raw);
    board_gpio_write_do(FDD_WPROTC, outputs->wprotc_raw);
    board_gpio_write_do(FDD_RDDA, outputs->rdda_raw);
}

static void toshiba_fdd_iface_read_inputs_raw(toshiba_fdd_inputs_t *inputs)
{
    if (inputs == NULL)
    {
        return;
    }

    inputs->fd_sela_raw = board_gpio_read_di(FDD_FDSELA);
    inputs->mona_raw = board_gpio_read_di(FDD_MONA);
    inputs->lowdns_raw = board_gpio_read_di(FDD_LOWDNS);
    inputs->fdcdrc_raw = board_gpio_read_di(FDD_FDCDRC);
    inputs->step_raw = board_gpio_read_di(FDD_STEP);
    inputs->wgate_raw = board_gpio_read_di(FDD_WGATE);
    inputs->side_raw = board_gpio_read_di(FDD_SIDE);
    inputs->wdata_raw = board_gpio_read_di(FDD_WDATA);
}

static uint32_t toshiba_fdd_iface_diff_inputs(const toshiba_fdd_inputs_t *old_inputs,
                                              const toshiba_fdd_inputs_t *new_inputs)
{
    uint32_t changed_mask = TOSHIBA_FDD_CHANGED_NONE;

    if ((old_inputs == NULL) || (new_inputs == NULL))
    {
        return changed_mask;
    }

    if (old_inputs->fd_sela_raw != new_inputs->fd_sela_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_FDSELA;
    }

    if (old_inputs->mona_raw != new_inputs->mona_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_MONA;
    }

    if (old_inputs->lowdns_raw != new_inputs->lowdns_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_LOWDNS;
    }

    if (old_inputs->fdcdrc_raw != new_inputs->fdcdrc_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_FDCDRC;
    }

    if (old_inputs->step_raw != new_inputs->step_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_STEP;
    }

    if (old_inputs->wgate_raw != new_inputs->wgate_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_WGATE;
    }

    if (old_inputs->side_raw != new_inputs->side_raw)
    {
        changed_mask |= TOSHIBA_FDD_CHANGED_SIDE;
    }

    return changed_mask;
}

void toshiba_fdd_iface_load_initial_guess_polarity(
    toshiba_fdd_signal_polarity_t *polarity)
{
    if (polarity == NULL)
    {
        return;
    }

    memset(polarity, 0, sizeof(*polarity));

    /* probable / initial_guess */
    toshiba_fdd_iface_configure_signal(&polarity->fd_sela_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->mona_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->lowdns_active, true, true);
    toshiba_fdd_iface_configure_signal(&polarity->step_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->wgate_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->side_1_level, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->index_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->dskchg_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->ready_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->track0_active, true, false);
    toshiba_fdd_iface_configure_signal(&polarity->wprotc_active, true, false);

    /* pending */
    toshiba_fdd_iface_configure_signal(&polarity->fdcdrc_towards_center, false, false);
    toshiba_fdd_iface_configure_signal(&polarity->media_active, false, false);
    toshiba_fdd_iface_configure_signal(&polarity->rdda_active, false, false);
}

void toshiba_fdd_iface_init(toshiba_fdd_iface_t *iface)
{
    if (iface == NULL)
    {
        return;
    }

    memset(iface, 0, sizeof(*iface));
    toshiba_fdd_iface_read_inputs_raw(&iface->inputs);
    iface->last_poll_us = get_us();
    iface->is_initialized = true;
}

uint32_t toshiba_fdd_iface_poll_inputs(toshiba_fdd_iface_t *iface)
{
    toshiba_fdd_inputs_t inputs_now;
    uint32_t now_us;
    uint32_t changed_mask;

    if ((iface == NULL) || !iface->is_initialized)
    {
        return TOSHIBA_FDD_CHANGED_NONE;
    }

    now_us = get_us();
    if ((uint32_t)(now_us - iface->last_poll_us) < TOSHIBA_FDD_POLL_INTERVAL_US)
    {
        return TOSHIBA_FDD_CHANGED_NONE;
    }

    iface->last_poll_us = now_us;
    toshiba_fdd_iface_read_inputs_raw(&inputs_now);
    changed_mask = toshiba_fdd_iface_diff_inputs(&iface->inputs, &inputs_now);
    iface->inputs = inputs_now;

    return changed_mask;
}

const toshiba_fdd_inputs_t *toshiba_fdd_iface_get_inputs(const toshiba_fdd_iface_t *iface)
{
    if (iface == NULL)
    {
        return NULL;
    }

    return &iface->inputs;
}

void toshiba_fdd_iface_log_inputs(const toshiba_fdd_iface_t *iface, uint32_t changed_mask)
{
    const toshiba_fdd_inputs_t *inputs;

    if ((iface == NULL) || !iface->is_initialized)
    {
        return;
    }

    inputs = &iface->inputs;

    DEBUG_SERIAL_LOG("pj5 raw @%luus", (unsigned long)get_us());

    if ((changed_mask & TOSHIBA_FDD_CHANGED_FDSELA) != 0u)
    {
        DEBUG_SERIAL_LOG(" fd_sela=%u", (unsigned int)inputs->fd_sela_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_MONA) != 0u)
    {
        DEBUG_SERIAL_LOG(" mona=%u", (unsigned int)inputs->mona_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_LOWDNS) != 0u)
    {
        DEBUG_SERIAL_LOG(" lowdns=%u", (unsigned int)inputs->lowdns_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_FDCDRC) != 0u)
    {
        DEBUG_SERIAL_LOG(" fdcdrc=%u", (unsigned int)inputs->fdcdrc_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_STEP) != 0u)
    {
        DEBUG_SERIAL_LOG(" step=%u", (unsigned int)inputs->step_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_WGATE) != 0u)
    {
        DEBUG_SERIAL_LOG(" wgate=%u", (unsigned int)inputs->wgate_raw);
    }

    if ((changed_mask & TOSHIBA_FDD_CHANGED_SIDE) != 0u)
    {
        DEBUG_SERIAL_LOG(" side=%u", (unsigned int)inputs->side_raw);
    }

    DEBUG_SERIAL_LOG("\r\n");
}

bool toshiba_fdd_iface_decode_inputs(const toshiba_fdd_iface_t *iface,
                                     const toshiba_fdd_signal_polarity_t *polarity,
                                     toshiba_fdd_logical_inputs_t *logical_inputs)
{
    if ((iface == NULL)
        || (polarity == NULL)
        || (logical_inputs == NULL)
        || !iface->is_initialized)
    {
        return false;
    }

    memset(logical_inputs, 0, sizeof(*logical_inputs));
    toshiba_fdd_iface_decode_signal(iface->inputs.fd_sela_raw,
                                    &polarity->fd_sela_active,
                                    &logical_inputs->drive_select,
                                    &logical_inputs->drive_select_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.mona_raw,
                                    &polarity->mona_active,
                                    &logical_inputs->motor_on,
                                    &logical_inputs->motor_on_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.lowdns_raw,
                                    &polarity->lowdns_active,
                                    &logical_inputs->low_density,
                                    &logical_inputs->low_density_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.step_raw,
                                    &polarity->step_active,
                                    &logical_inputs->step_active,
                                    &logical_inputs->step_active_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.fdcdrc_raw,
                                    &polarity->fdcdrc_towards_center,
                                    &logical_inputs->direction_towards_center,
                                    &logical_inputs->direction_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.wgate_raw,
                                    &polarity->wgate_active,
                                    &logical_inputs->write_gate,
                                    &logical_inputs->write_gate_valid);
    toshiba_fdd_iface_decode_signal(iface->inputs.side_raw,
                                    &polarity->side_1_level,
                                    &logical_inputs->side_1_selected,
                                    &logical_inputs->side_valid);

    return true;
}

bool toshiba_fdd_iface_write_outputs(const toshiba_fdd_logical_outputs_t *logical_outputs,
                                     const toshiba_fdd_signal_polarity_t *polarity)
{
    toshiba_fdd_outputs_t raw_outputs;

    if ((logical_outputs == NULL) || (polarity == NULL))
    {
        return false;
    }

    toshiba_fdd_iface_read_outputs_raw(&raw_outputs);
    toshiba_fdd_iface_apply_signal(logical_outputs->index_active,
                                   logical_outputs->index_valid,
                                   &polarity->index_active,
                                   &raw_outputs.index_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->dskchg_active,
                                   logical_outputs->dskchg_valid,
                                   &polarity->dskchg_active,
                                   &raw_outputs.dskchg_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->ready_active,
                                   logical_outputs->ready_valid,
                                   &polarity->ready_active,
                                   &raw_outputs.ready_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->media_active,
                                   logical_outputs->media_valid,
                                   &polarity->media_active,
                                   &raw_outputs.media_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->track0_active,
                                   logical_outputs->track0_valid,
                                   &polarity->track0_active,
                                   &raw_outputs.track0_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->wprotc_active,
                                   logical_outputs->wprotc_valid,
                                   &polarity->wprotc_active,
                                   &raw_outputs.wprotc_raw);
    toshiba_fdd_iface_apply_signal(logical_outputs->rdda_active,
                                   logical_outputs->rdda_valid,
                                   &polarity->rdda_active,
                                   &raw_outputs.rdda_raw);

    toshiba_fdd_iface_write_outputs_raw(&raw_outputs);

    return true;
}
