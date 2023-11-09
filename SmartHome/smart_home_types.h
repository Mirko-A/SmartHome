#ifndef SMART_HOME_TYPES_H
#define SMART_HOME_TYPES_H

enum class LightLocation
{
    LIVING_ROOM = 0x00,
    BEDROOM,
    KITCHEN,
    LIGHT_LOCATION_CNT,
};

enum class ACMode
{
    NORMAL = 0x00,
    FAST,
    TURBO,
    AC_MODE_CNT,
};

#endif // SMART_HOME_TYPES_H
