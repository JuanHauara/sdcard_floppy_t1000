#ifndef FLOPPY_DRIVE_H
#define FLOPPY_DRIVE_H


#include <stdbool.h>
#include <stdint.h>

#include "floppy_emu.h"


typedef struct
{
    bool media_present;
    bool write_protected;
    bool disk_changed;
    bool selected;
    bool motor_on;
    uint16_t cylinder;
    uint16_t max_cylinder;
    uint8_t head;
    uint8_t head_count;
} floppy_drive_t;

typedef struct
{
    bool media_present;
    bool write_protected;
    bool disk_changed;
    bool selected;
    bool motor_on;
    bool track0;
    uint16_t cylinder;
    uint8_t head;
} floppy_drive_status_t;


void floppy_drive_init(floppy_drive_t *drive, const floppy_emu_t *emu);
void floppy_drive_reset(floppy_drive_t *drive);
bool floppy_drive_set_head(floppy_drive_t *drive, uint8_t head);
void floppy_drive_set_selected(floppy_drive_t *drive, bool selected);
void floppy_drive_set_motor_on(floppy_drive_t *drive, bool motor_on);
bool floppy_drive_step_towards_center(floppy_drive_t *drive);
bool floppy_drive_step_towards_track0(floppy_drive_t *drive);
void floppy_drive_set_disk_changed(floppy_drive_t *drive, bool disk_changed);
void floppy_drive_get_status(const floppy_drive_t *drive, floppy_drive_status_t *status);
floppy_emu_status_t floppy_drive_build_current_track(const floppy_drive_t *drive,
                                                     floppy_emu_t *emu,
                                                     uint16_t *out_words,
                                                     uint32_t max_words,
                                                     floppy_emu_track_info_t *out_info);


#endif  // FLOPPY_DRIVE_H
