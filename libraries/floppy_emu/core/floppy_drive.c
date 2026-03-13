#include "floppy_drive.h"

#include <string.h>


static void floppy_drive_load_geometry(floppy_drive_t *drive, const floppy_emu_t *emu)
{
    if ((drive == NULL) || (emu == NULL) || !floppy_emu_is_ready(emu))
    {
        return;
    }

    drive->media_present = true;
    drive->write_protected = emu->write_protected;
    drive->max_cylinder = (emu->geometry.cylinders > 0u)
        ? (uint16_t)(emu->geometry.cylinders - 1u)
        : 0u;
    drive->head_count = emu->geometry.heads;
}

void floppy_drive_init(floppy_drive_t *drive, const floppy_emu_t *emu)
{
    if (drive == NULL)
    {
        return;
    }

    memset(drive, 0, sizeof(*drive));
    floppy_drive_load_geometry(drive, emu);
}

void floppy_drive_reset(floppy_drive_t *drive)
{
    if (drive == NULL)
    {
        return;
    }

    drive->selected = false;
    drive->motor_on = false;
    drive->cylinder = 0u;
    drive->head = 0u;
}

bool floppy_drive_set_head(floppy_drive_t *drive, uint8_t head)
{
    if ((drive == NULL) || !drive->media_present)
    {
        return false;
    }

    if (head >= drive->head_count)
    {
        return false;
    }

    drive->head = head;

    return true;
}

void floppy_drive_set_selected(floppy_drive_t *drive, bool selected)
{
    if (drive == NULL)
    {
        return;
    }

    drive->selected = selected;
}

void floppy_drive_set_motor_on(floppy_drive_t *drive, bool motor_on)
{
    if (drive == NULL)
    {
        return;
    }

    drive->motor_on = motor_on;
}

bool floppy_drive_step_towards_center(floppy_drive_t *drive)
{
    if ((drive == NULL) || !drive->media_present)
    {
        return false;
    }

    if (drive->cylinder >= drive->max_cylinder)
    {
        return false;
    }

    drive->cylinder++;

    return true;
}

bool floppy_drive_step_towards_track0(floppy_drive_t *drive)
{
    if ((drive == NULL) || !drive->media_present)
    {
        return false;
    }

    if (drive->cylinder == 0u)
    {
        return false;
    }

    drive->cylinder--;

    return true;
}

void floppy_drive_set_disk_changed(floppy_drive_t *drive, bool disk_changed)
{
    if (drive == NULL)
    {
        return;
    }

    drive->disk_changed = disk_changed;
}

void floppy_drive_get_status(const floppy_drive_t *drive, floppy_drive_status_t *status)
{
    if ((drive == NULL) || (status == NULL))
    {
        return;
    }

    memset(status, 0, sizeof(*status));
    status->media_present = drive->media_present;
    status->write_protected = drive->write_protected;
    status->disk_changed = drive->disk_changed;
    status->selected = drive->selected;
    status->motor_on = drive->motor_on;
    status->track0 = (drive->cylinder == 0u);
    status->cylinder = drive->cylinder;
    status->head = drive->head;
}

floppy_emu_status_t floppy_drive_build_current_track(const floppy_drive_t *drive,
                                                     floppy_emu_t *emu,
                                                     uint16_t *out_words,
                                                     uint32_t max_words,
                                                     floppy_emu_track_info_t *out_info)
{
    if ((drive == NULL) || (emu == NULL) || (out_words == NULL))
    {
        return FLOPPY_EMU_STATUS_INVALID_ARG;
    }

    if (!drive->media_present)
    {
        return FLOPPY_EMU_STATUS_NOT_MOUNTED;
    }

    return floppy_emu_build_mfm_track(emu,
                                      drive->cylinder,
                                      drive->head,
                                      out_words,
                                      max_words,
                                      out_info);
}
