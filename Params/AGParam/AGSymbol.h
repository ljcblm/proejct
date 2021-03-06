#pragma once
#include <QString>
#include "AGDefine.h"

class AGSymbol
{
public:
    static const char *convert(AGTypeGas index)
    {
        static const char *symbol[AG_TYPE_NR] =
        {
            "CO2", "N2O", "AA1", "AA2", "O2"
        };

        return symbol[index];
    }

    static const char *convert(AGSweepSpeed index)
    {
        static const char *symbol[AG_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(AGAnaestheticType index)
    {
        static const char *symbol[AG_ANAESTHETIC_NR] =
        {
            "NO", "HAL", "ENF", "ISO", "SEV", "DES"
        };

        return symbol[index];
    }

    static const char *convert(AGLimitAlarmType index)
    {
        static const char *symbol[AG_LIMIT_ALARM_NR] =
        {
            "AGEtCO2Low", "AGEtCO2High",
            "AGFiCO2Low", "AGFiCO2High",

            "AGEtN2OLow", "AGEtN2OHigh",
            "AGFiN2OLow", "AGFiN2OHigh",

            "AGEtAA1Low", "AGEtAA1High",
            "AGFiAA1Low", "AGFiAA1High",

            "AGEtAA2Low", "AGEtAA2High",
            "AGFiAA2Low", "AGFiAA2High",

            "AGEtO2Low", "AGEtO2High",
            "AGFiO2Low", "AGFiO2High",
        };
        return symbol[index];
    }

    static const char *convert(AGOneShotType index)
    {
        static const char *symbol[AG_ONESHOT_NR] =
        {

        };
        return symbol[index];
    }

    static const QString &convert(AGModuleSwitch index)
    {
        static const QString symbol[AG_MODULE_SWITCH_NR] =
        {
          "Enable", "Disable"
        };
        return symbol[index];
    }

    static const QString &convert(AGModuleCO2Option index)
    {
        static const QString symbol[AG_MODULE_CO2_OPTION_NR] =
        {
            "SetUp"
        };
        return symbol[index];
    }

    static const QString &convert(AGModuleN2OOption index)
    {
        static const QString symbol[AG_MODULE_N2O_OPTION_NR] =
        {
            "SetUp"
        };
        return symbol[index];
    }

    static const QString &convert(AGModuleAA1Option index)
    {
        static const QString symbol[AG_MODULE_AA1_OPTION_NR] =
        {
            "SetUp"
        };
        return symbol[index];
    }

    static const QString &convert(AGModuleAA2Option index)
    {
        static const QString symbol[AG_MODULE_AA2_OPTION_NR] =
        {
            "SetUp"
        };
        return symbol[index];
    }

    static const QString &convert(AGModuleO2Option index)
    {
        static const QString symbol[AG_MODULE_O2_OPTION_NR] =
        {
            "SetUp"
        };
        return symbol[index];
    }

};
