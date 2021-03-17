#include "beoremote_one_types.h"

#include <stdio.h>

#define NEWLINE_REPLACE_CHAR ','

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

size_t beoremote_one_attrib_size(enum beoremote_one_attrib attrib)
{
    size_t attrib_size;

    switch (attrib) {
    case BEOREMOTE_ONE_ATTRIB_DISC_TRACK:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE:
    case BEOREMOTE_ONE_ATTRIB_CINEMA_MODE:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE:
    case BEOREMOTE_ONE_ATTRIB_CONTROL_1:
    case BEOREMOTE_ONE_ATTRIB_CONTROL_2:
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT:
    case BEOREMOTE_ONE_ATTRIB_VOLUME:
        attrib_size = 1;
        break;

    case BEOREMOTE_ONE_ATTRIB_FEATURES:
    case BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED:
        attrib_size = 16;
        break;

    default:
        attrib_size = BEOREMOTE_ONE_ATTRIB_MAX_SIZE;
        break;
    }

    return attrib_size;
}

#pragma GCC diagnostic pop

const char *beoremote_one_attrib_to_string(enum beoremote_one_attrib attrib)
{
    switch (attrib) {
    case BEOREMOTE_ONE_ATTRIB_VERSION:
        return "BEOREMOTE_ONE_ATTRIB_VERSION";
    case BEOREMOTE_ONE_ATTRIB_FEATURES:
        return "BEOREMOTE_ONE_ATTRIB_FEATURES";
    case BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED:
        return "BEOREMOTE_ONE_ATTRIB_FEATURES_CHANGED";
    case BEOREMOTE_ONE_ATTRIB_INJECT_PRESS:
        return "BEOREMOTE_ONE_ATTRIB_INJECT_PRESS";
    case BEOREMOTE_ONE_ATTRIB_INJECT_RELEASE:
        return "BEOREMOTE_ONE_ATTRIB_INJECT_RELEASE";
    case BEOREMOTE_ONE_ATTRIB_DISC_TRACK:
        return "BEOREMOTE_ONE_ATTRIB_DISC_TRACK";
    case BEOREMOTE_ONE_ATTRIB_STAND_POSITIONS:
        return "BEOREMOTE_ONE_ATTRIB_STAND_POSITIONS";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_STAND_POSITION";
    case BEOREMOTE_ONE_ATTRIB_SPEAKER_GROUPS:
        return "BEOREMOTE_ONE_ATTRIB_SPEAKER_GROUPS";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_SPEAKER_GROUP";
    case BEOREMOTE_ONE_ATTRIB_SOUND_MODES:
        return "BEOREMOTE_ONE_ATTRIB_SOUND_MODES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_SOUND_MODE";
    case BEOREMOTE_ONE_ATTRIB_PICTURE_FORMATS:
        return "BEOREMOTE_ONE_ATTRIB_PICTURE_FORMATS";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_FORMAT";
    case BEOREMOTE_ONE_ATTRIB_PICTURE_MODES:
        return "BEOREMOTE_ONE_ATTRIB_PICTURE_MODES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_PICTURE_MODE";
    case BEOREMOTE_ONE_ATTRIB_PICTURE_MUTE:
        return "BEOREMOTE_ONE_ATTRIB_PICTURE_MUTE";
    case BEOREMOTE_ONE_ATTRIB_2D_3D_MODES:
        return "BEOREMOTE_ONE_ATTRIB_2D_3D_MODES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_2D_3D_MODE";
    case BEOREMOTE_ONE_ATTRIB_TV_SOURCES:
        return "BEOREMOTE_ONE_ATTRIB_TV_SOURCES";
    case BEOREMOTE_ONE_ATTRIB_MUSIC_SOURCES:
        return "BEOREMOTE_ONE_ATTRIB_MUSIC_SOURCES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE";
    case BEOREMOTE_ONE_ATTRIB_CUSTOM_COMMANDS:
        return "BEOREMOTE_ONE_ATTRIB_CUSTOM_COMMANDS";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_CUSTOM_COMMAND";
    case BEOREMOTE_ONE_ATTRIB_HOME_CONTROL_SCENES:
        return "BEOREMOTE_ONE_ATTRIB_HOME_CONTROL_SCENES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_HOME_CONTROL_SCENE";
    case BEOREMOTE_ONE_ATTRIB_CINEMA_MODE:
        return "BEOREMOTE_ONE_ATTRIB_CINEMA_MODE";
    case BEOREMOTE_ONE_ATTRIB_EXPERIENCES:
        return "BEOREMOTE_ONE_ATTRIB_EXPERIENCES";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_EXPERIENCE";
    case BEOREMOTE_ONE_ATTRIB_CONTROL_1:
        return "BEOREMOTE_ONE_ATTRIB_CONTROL_1";
    case BEOREMOTE_ONE_ATTRIB_CONTROL_2:
        return "BEOREMOTE_ONE_ATTRIB_CONTROL_2";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_1:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_1";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_2:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_2";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_3:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_3";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_4:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_4";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_5:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_5";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_6:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_6";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_7:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_7";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_8:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_8";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_9:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_9";
    case BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_10:
        return "BEOREMOTE_ONE_ATTRIB_SOURCE_CONTENT_10";
    case BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT:
        return "BEOREMOTE_ONE_ATTRIB_ACTIVE_SOURCE_CONTENT";
    case BEOREMOTE_ONE_ATTRIB_MY_BUTTONS:
        return "BEOREMOTE_ONE_ATTRIB_MY_BUTTONS";
    case BEOREMOTE_ONE_ATTRIB_VOLUME:
        return "BEOREMOTE_ONE_ATTRIB_VOLUME";
    case BEOREMOTE_ONE_ATTRIB_MAX:
        return "BEOREMOTE_ONE_ATTRIB_MAX";
    }
    return "beoremote_one_attrib enum has not been mapped!";
}

char *beoremote_one_attrib_data_to_string(enum beoremote_one_attrib attrib, const uint8_t *data, size_t data_size, char *buffer, size_t buffer_size)
{
    char *src = (char *)data;
    const char *src_end = src + data_size;
    char *dest = buffer;
    const char *dest_end = dest + buffer_size;
    static const size_t PRINT_BUFFER_SIZE = 6; // Size of ',0xNN' incl. zero-terminator

    if (beoremote_one_attrib_size(attrib) == BEOREMOTE_ONE_ATTRIB_MAX_SIZE) {
        // Copy every char in data, replace each newline with NEWLINE_REPLACE_CHAR
        while (dest < dest_end && src < src_end && *src != '\0') {
            if (*src == '\n')
                *dest = NEWLINE_REPLACE_CHAR;
            else
                *dest = *src;
            ++src;
            ++dest;
        }
        if (dest < dest_end)
            *dest = '\0';
        else if (dest > buffer) // Replace last char in buffer with zero-terminator
            *--dest = '\0';
    } else {
        if ((data_size == 0 || buffer_size < PRINT_BUFFER_SIZE - 1) && buffer_size > 0)
            *buffer = '\0';
        else {
            // Print first byte without leading comma
            if (buffer_size >= PRINT_BUFFER_SIZE - 1 && src < src_end)
                sprintf(dest, "0x%02x", *src);
            src += 1;
            dest += PRINT_BUFFER_SIZE - 2;
            // Print next bytes with leading comma
            while (src < src_end && dest + PRINT_BUFFER_SIZE <= dest_end) {
                sprintf(dest, ",0x%02x", *src);
                src += 1;
                dest += PRINT_BUFFER_SIZE - 1;
            }
        }
    }

    return buffer;
}
