#ifndef TOSHIBA_FDD_IFACE_H
#define TOSHIBA_FDD_IFACE_H


#include <stdbool.h>
#include <stdint.h>


typedef enum
{
    TOSHIBA_FDD_CHANGED_NONE = 0,
    TOSHIBA_FDD_CHANGED_FDSELA = (1u << 0),
    TOSHIBA_FDD_CHANGED_MONA = (1u << 1),
    TOSHIBA_FDD_CHANGED_LOWDNS = (1u << 2),
    TOSHIBA_FDD_CHANGED_FDCDRC = (1u << 3),
    TOSHIBA_FDD_CHANGED_STEP = (1u << 4),
    TOSHIBA_FDD_CHANGED_WGATE = (1u << 5),
    TOSHIBA_FDD_CHANGED_SIDE = (1u << 6),
} toshiba_fdd_changed_mask_t;

typedef struct
{
    bool fd_sela_raw;
    bool mona_raw;
    bool lowdns_raw;
    bool fdcdrc_raw;
    bool step_raw;
    bool wgate_raw;
    bool side_raw;
    bool wdata_raw;
} toshiba_fdd_inputs_t;

typedef struct
{
    bool index_raw;
    bool dskchg_raw;
    bool ready_raw;
    bool media_raw;
    bool track0_raw;
    bool wprotc_raw;
    bool rdda_raw;
} toshiba_fdd_outputs_t;

typedef struct
{
    bool is_configured;
    bool level;
} toshiba_fdd_signal_level_t;

typedef struct
{
    bool drive_select;
    bool drive_select_valid;
    bool motor_on;
    bool motor_on_valid;
    bool low_density;
    bool low_density_valid;
    bool step_active;
    bool step_active_valid;
    bool direction_towards_center;
    bool direction_valid;
    bool write_gate;
    bool write_gate_valid;
    bool side_1_selected;
    bool side_valid;
} toshiba_fdd_logical_inputs_t;

typedef struct
{
    bool index_active;
    bool index_valid;
    bool dskchg_active;
    bool dskchg_valid;
    bool ready_active;
    bool ready_valid;
    bool media_active;
    bool media_valid;
    bool track0_active;
    bool track0_valid;
    bool wprotc_active;
    bool wprotc_valid;
    bool rdda_active;
    bool rdda_valid;
} toshiba_fdd_logical_outputs_t;

typedef struct
{
    toshiba_fdd_signal_level_t fd_sela_active;
    toshiba_fdd_signal_level_t mona_active;
    toshiba_fdd_signal_level_t lowdns_active;
    toshiba_fdd_signal_level_t step_active;
    toshiba_fdd_signal_level_t fdcdrc_towards_center;
    toshiba_fdd_signal_level_t wgate_active;
    toshiba_fdd_signal_level_t side_1_level;
    toshiba_fdd_signal_level_t index_active;
    toshiba_fdd_signal_level_t dskchg_active;
    toshiba_fdd_signal_level_t ready_active;
    toshiba_fdd_signal_level_t media_active;
    toshiba_fdd_signal_level_t track0_active;
    toshiba_fdd_signal_level_t wprotc_active;
    toshiba_fdd_signal_level_t rdda_active;
} toshiba_fdd_signal_polarity_t;

typedef struct
{
    toshiba_fdd_inputs_t inputs;
    uint32_t last_poll_us;
    bool is_initialized;
} toshiba_fdd_iface_t;


void toshiba_fdd_iface_load_initial_guess_polarity(
    toshiba_fdd_signal_polarity_t *polarity);
void toshiba_fdd_iface_init(toshiba_fdd_iface_t *iface);
uint32_t toshiba_fdd_iface_poll_inputs(toshiba_fdd_iface_t *iface);
const toshiba_fdd_inputs_t *toshiba_fdd_iface_get_inputs(const toshiba_fdd_iface_t *iface);
void toshiba_fdd_iface_log_inputs(const toshiba_fdd_iface_t *iface, uint32_t changed_mask);
bool toshiba_fdd_iface_decode_inputs(const toshiba_fdd_iface_t *iface,
                                     const toshiba_fdd_signal_polarity_t *polarity,
                                     toshiba_fdd_logical_inputs_t *logical_inputs);
bool toshiba_fdd_iface_write_outputs(const toshiba_fdd_logical_outputs_t *logical_outputs,
                                     const toshiba_fdd_signal_polarity_t *polarity);


#endif  // TOSHIBA_FDD_IFACE_H
