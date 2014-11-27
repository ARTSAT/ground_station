/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++11
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 6.1 (Apple LLVM 6.0)
**
**      TGSTelemetryINVADER.cpp
**
**      ------------------------------------------------------------------------
**
**      The MIT License (MIT)
**
**      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
**      associated documentation files (the "Software"), to deal in the Software without restriction,
**      including without limitation the rights to use, copy, modify, merge, publish, distribute,
**      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
**      furnished to do so, subject to the following conditions:
**      The above copyright notice and this permission notice shall be included in all copies or
**      substantial portions of the Software.
**      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
**      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
**      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
**      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
**      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
**      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
**      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
**      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
**      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
**      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
**      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
**      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/

#include "TGSTelemetryINVADER.h"

namespace tgs {

#define CONVERT(param, result, field, function) (function((param), &BinaryRec::field, #field, &(result)->field))
#define NORMALIZE(field)                        (normalize(&TelemetryRec::field))

static  TGSTelemetryINVADER::BinaryRec const    g_binaryMinimum = {
      {     // obctime_OBCTime
          0,
          0,
          0,
          0
      },
      0,    // voltage_Battery
      0,    // voltage_Bus
      0,    // voltage_Solar
      0,    // reserved_008
      0,    // reserved_009
      0,    // reserved_010
      0,    // ampare_PowerCPUBoard
      0,    // ampare_Bus
      0,    // ampare_Battery
      0,    // ampare_Solar
      0,    // ampare_SolarMY2
      0,    // ampare_SolarPY2
      0,    // ampare_SolarMZ
      0,    // ampare_SolarPZ
      0,    // ampare_SolarMY1
      0,    // ampare_SolarPY1
      0,    // ampare_SolarMX
      0,    // ampare_SolarPX
      0,    // ampare_Antenna
      0,    // ampare_Heater
      0,    // ampare_TX
      0,    // ampare_CW
      0,    // ampare_RX
      0,    // ampare_MainCPU
      0,    // ampare_MissionCPU
      0,    // reserved_030
     48,    // temperature_Battery1
     48,    // temperature_Battery2
     48,    // temperature_Battery3
      0,    // temperature_SolarMX1
      0,    // temperature_SolarMX2
      0,    // temperature_SolarPY1
      0,    // temperature_SolarPY2
      0,    // temperature_SolarMY1
      0,    // temperature_SolarMY2
      0,    // temperature_SolarPX
      0,    // temperature_SolarPZ2
      0,    // temperature_SolarMZ1
      0,    // temperature_SolarMZ2
      0,    // temperature_PowerCPUBoard
      0,    // temperature_MainCPU
     24,    // temperature_TXCW
     24,    // temperature_RX
      0,    // temperature_MainCPUBoard
      0,    // reserved_049
      0,    // reserved_050
      0,    // gyro_X
      0,    // gyro_Y
      0,    // gyro_Z
      0,    // magnet_X
      0,    // magnet_Y
      0,    // magnet_Z
      0,    // reserved_057
      0,    // reserved_058
      0,    // reserved_059
      0,    // rssi_RSSI
      0,    // reserved_061
      0,    // mission_status_UserReprogramModeEnable
      0,    // mission_status_BootProgramID
      {     // mission_status_MissionCPUONIntervalTime
          0,
          0
      },
      {     // mission_status_MissionCPUONOFFPeriod
          0,
          0
      },
      0,    // mission_status_MissionCPUONDutyCycle
      0,    // mission_status_ScheduledBootProgramID
      {     // mission_status_ScheduledBootTime
          0,
          0
      },
      0,    // mission_status_MissionCPUReprogramCount
      0,    // mission_status_MissionCPUStartupCount
      0,    // mission_status_MissionCPUShutdownCount
      {     // mission_status_SharedMemoryWritePointer
          0,
          0,
          0
      },
      0,    // reserved_078
      0,    // reserved_079
      0,    // reserved_080
      {     // power_status_reserved
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0
      },
      0,    // reserved_089
      0,    // reserved_090
      0,    // reserved_091
      {     // init_status_MainCPUInternalFunction
          0,
          0
      },
      {     // init_status_MainCPUInternalRegister
          0,
          0
      },
      {     // init_status_MainCPUPeripheralDevice
          0,
          0
      },
      0,    // reserved_098
      0,    // reserved_099
      0,    // mode_MainCPU
      0,    // chargeflag_ChargeICR
      0,    // chargeflag_ChargeICG
      0,    // flag_Flags
      0,    // flag_reserved
      0,    // reserved_105
      0,    // error_counter_MainBoardSEL
      0,    // error_counter_MainCPUSEU
      0,    // error_counter_MainCPUSelfResetRequest
      0,    // error_counter_PowerBoardSEL
      0,    // error_counter_PowerCPUSEU
      0,    // error_counter_MissionBoardSEL
      0,    // error_counter_MissionCPUShutdownNoResponse
      0,    // error_counter_RadioBoardSEL
      0,    // reserved_114
      {     // message_counter_SCI0CommandReceive
          0,
          0
      },
      {     // message_counter_SCI0CommandSend
          0,
          0
      },
      {     // message_counter_SCI2CommandReceive
          0,
          0
      },
      {     // message_counter_SCI2CommandSend
          0,
          0
      },
      {     // message_counter_RXCommandReceive
          0,
          0
      },
      {     // message_counter_TXCommandSend
          0,
          0
      },
      0     // reserved_127
};
static  TGSTelemetryINVADER::BinaryRec const    g_binaryMaximum = {
      {     // obctime_OBCTime
        255,
        255,
        255,
        255
      },
    255,    // voltage_Battery
    255,    // voltage_Bus
    255,    // voltage_Solar
      0,    // reserved_008
      0,    // reserved_009
      0,    // reserved_010
    122,    // ampare_PowerCPUBoard
    122,    // ampare_Bus
    102,    // ampare_Battery
    102,    // ampare_Solar
    102,    // ampare_SolarMY2
    102,    // ampare_SolarPY2
    102,    // ampare_SolarMZ
    102,    // ampare_SolarPZ
    102,    // ampare_SolarMY1
    102,    // ampare_SolarPY1
    102,    // ampare_SolarMX
    102,    // ampare_SolarPX
    122,    // ampare_Antenna
    102,    // ampare_Heater
    122,    // ampare_TX
    102,    // ampare_CW
    102,    // ampare_RX
    102,    // ampare_MainCPU
    102,    // ampare_MissionCPU
      0,    // reserved_030
    191,    // temperature_Battery1
    191,    // temperature_Battery2
    191,    // temperature_Battery3
    255,    // temperature_SolarMX1
    255,    // temperature_SolarMX2
    255,    // temperature_SolarPY1
    255,    // temperature_SolarPY2
    255,    // temperature_SolarMY1
    255,    // temperature_SolarMY2
    255,    // temperature_SolarPX
    255,    // temperature_SolarPZ2
    255,    // temperature_SolarMZ1
    255,    // temperature_SolarMZ2
     72,    // temperature_PowerCPUBoard
    255,    // temperature_MainCPU
    237,    // temperature_TXCW
    237,    // temperature_RX
     72,    // temperature_MainCPUBoard
      0,    // reserved_049
      0,    // reserved_050
    130,    // gyro_X
    130,    // gyro_Y
    130,    // gyro_Z
    240,    // magnet_X
    247,    // magnet_Y
    232,    // magnet_Z
      0,    // reserved_057
      0,    // reserved_058
      0,    // reserved_059
    255,    // rssi_RSSI
      0,    // reserved_061
    255,    // mission_status_UserReprogramModeEnable
    255,    // mission_status_BootProgramID
      {     // mission_status_MissionCPUONIntervalTime
        255,
        255
      },
      {     // mission_status_MissionCPUONOFFPeriod
        255,
        255
      },
    255,    // mission_status_MissionCPUONDutyCycle
    255,    // mission_status_ScheduledBootProgramID
      {     // mission_status_ScheduledBootTime
        255,
        255
      },
    255,    // mission_status_MissionCPUReprogramCount
    255,    // mission_status_MissionCPUStartupCount
    255,    // mission_status_MissionCPUShutdownCount
      {     // mission_status_SharedMemoryWritePointer
        255,
        255,
        255
      },
      0,    // reserved_078
      0,    // reserved_079
      0,    // reserved_080
      {     // power_status_reserved
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255
      },
      0,    // reserved_089
      0,    // reserved_090
      0,    // reserved_091
      {     // init_status_MainCPUInternalFunction
        255,
        255
      },
      {     // init_status_MainCPUInternalRegister
        255,
        255
      },
      {     // init_status_MainCPUPeripheralDevice
        255,
        255
      },
      0,    // reserved_098
      0,    // reserved_099
    255,    // mode_MainCPU
    255,    // chargeflag_ChargeICR
    255,    // chargeflag_ChargeICG
    255,    // flag_Flags
    255,    // flag_reserved
      0,    // reserved_105
    255,    // error_counter_MainBoardSEL
    255,    // error_counter_MainCPUSEU
    255,    // error_counter_MainCPUSelfResetRequest
    255,    // error_counter_PowerBoardSEL
    255,    // error_counter_PowerCPUSEU
    255,    // error_counter_MissionBoardSEL
    255,    // error_counter_MissionCPUShutdownNoResponse
    255,    // error_counter_RadioBoardSEL
      0,    // reserved_114
      {     // message_counter_SCI0CommandReceive
        255,
        255
      },
      {     // message_counter_SCI0CommandSend
        255,
        255
      },
      {     // message_counter_SCI2CommandReceive
        255,
        255
      },
      {     // message_counter_SCI2CommandSend
        255,
        255
      },
      {     // message_counter_RXCommandReceive
        255,
        255
      },
      {     // message_counter_TXCommandSend
        255,
        255
      },
      0     // reserved_127
};
static  TGSTelemetryINVADER::TelemetryRec       g_telemetryMinimum = {
    0
};
static  TGSTelemetryINVADER::TelemetryRec       g_telemetryMaximum = {
    0
};
static  bool                                    g_initialize(false);

/*public static */TGSTelemetryINVADER::TelemetryRec TGSTelemetryINVADER::getMinimum(void)
{
    if (!g_initialize) {
        initialize();
        g_initialize = true;
    }
    return g_telemetryMinimum;
}

/*public static */TGSTelemetryINVADER::TelemetryRec TGSTelemetryINVADER::getMaximum(void)
{
    if (!g_initialize) {
        initialize();
        g_initialize = true;
    }
    return g_telemetryMaximum;
}

/*public static */TGSError TGSTelemetryINVADER::convert(BinaryRec const& param, TelemetryRec* result)
{
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        CONVERT(param, result,                            obctime_OBCTime,      convertDefault);
        CONVERT(param, result,                            voltage_Battery,      convertDefault);
        CONVERT(param, result,                                voltage_Bus,      convertDefault);
        CONVERT(param, result,                              voltage_Solar,      convertDefault);
        CONVERT(param, result,                       ampare_PowerCPUBoard,     convertCurrent1);
        CONVERT(param, result,                                 ampare_Bus,     convertCurrent2);
        CONVERT(param, result,                             ampare_Battery,     convertCurrent1);
        CONVERT(param, result,                               ampare_Solar,     convertCurrent1);
        CONVERT(param, result,                            ampare_SolarMY2,     convertCurrent1);
        CONVERT(param, result,                            ampare_SolarPY2,     convertCurrent1);
        CONVERT(param, result,                             ampare_SolarMZ,     convertCurrent1);
        CONVERT(param, result,                             ampare_SolarPZ,     convertCurrent1);
        CONVERT(param, result,                            ampare_SolarMY1,     convertCurrent1);
        CONVERT(param, result,                            ampare_SolarPY1,     convertCurrent1);
        CONVERT(param, result,                             ampare_SolarMX,     convertCurrent2);
        CONVERT(param, result,                             ampare_SolarPX,     convertCurrent1);
        CONVERT(param, result,                             ampare_Antenna,     convertCurrent2);
        CONVERT(param, result,                              ampare_Heater,     convertCurrent1);
        CONVERT(param, result,                                  ampare_TX,     convertCurrent1);
        CONVERT(param, result,                                  ampare_CW,     convertCurrent1);
        CONVERT(param, result,                                  ampare_RX,     convertCurrent1);
        CONVERT(param, result,                             ampare_MainCPU,     convertCurrent1);
        CONVERT(param, result,                          ampare_MissionCPU,     convertCurrent1);
        CONVERT(param, result,                       temperature_Battery1,  convertTemperature);
        CONVERT(param, result,                       temperature_Battery2,  convertTemperature);
        CONVERT(param, result,                       temperature_Battery3,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMX1,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMX2,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarPY1,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarPY2,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMY1,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMY2,  convertTemperature);
        CONVERT(param, result,                        temperature_SolarPX,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarPZ2,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMZ1,  convertTemperature);
        CONVERT(param, result,                       temperature_SolarMZ2,  convertTemperature);
        CONVERT(param, result,                  temperature_PowerCPUBoard,  convertTemperature);
        CONVERT(param, result,                        temperature_MainCPU,  convertTemperature);
        CONVERT(param, result,                           temperature_TXCW,  convertTemperature);
        CONVERT(param, result,                             temperature_RX,  convertTemperature);
        CONVERT(param, result,                   temperature_MainCPUBoard,  convertTemperature);
        CONVERT(param, result,                                     gyro_X,         convertGyro);
        CONVERT(param, result,                                     gyro_Y,         convertGyro);
        CONVERT(param, result,                                     gyro_Z,         convertGyro);
        CONVERT(param, result,                                   magnet_X, convertMagnetometer);
        CONVERT(param, result,                                   magnet_Y, convertMagnetometer);
        CONVERT(param, result,                                   magnet_Z, convertMagnetometer);
        CONVERT(param, result,                                  rssi_RSSI,      convertDefault);
        CONVERT(param, result,     mission_status_UserReprogramModeEnable,      convertDefault);
        CONVERT(param, result,               mission_status_BootProgramID,      convertDefault);
        CONVERT(param, result,    mission_status_MissionCPUONIntervalTime,      convertDefault);
        CONVERT(param, result,       mission_status_MissionCPUONOFFPeriod,      convertDefault);
        CONVERT(param, result,       mission_status_MissionCPUONDutyCycle,      convertDefault);
        CONVERT(param, result,      mission_status_ScheduledBootProgramID,      convertDefault);
        CONVERT(param, result,           mission_status_ScheduledBootTime,      convertDefault);
        CONVERT(param, result,    mission_status_MissionCPUReprogramCount,      convertDefault);
        CONVERT(param, result,      mission_status_MissionCPUStartupCount,      convertDefault);
        CONVERT(param, result,     mission_status_MissionCPUShutdownCount,      convertDefault);
        CONVERT(param, result,    mission_status_SharedMemoryWritePointer,      convertDefault);
        CONVERT(param, result,        init_status_MainCPUInternalFunction,      convertDefault);
        CONVERT(param, result,        init_status_MainCPUInternalRegister,      convertDefault);
        CONVERT(param, result,        init_status_MainCPUPeripheralDevice,      convertDefault);
        CONVERT(param, result,                               mode_MainCPU,      convertDefault);
        CONVERT(param, result,                       chargeflag_ChargeICR,      convertDefault);
        CONVERT(param, result,                       chargeflag_ChargeICG,      convertDefault);
        CONVERT(param, result,                                 flag_Flags,      convertDefault);
        CONVERT(param, result,                 error_counter_MainBoardSEL,      convertDefault);
        CONVERT(param, result,                   error_counter_MainCPUSEU,      convertDefault);
        CONVERT(param, result,      error_counter_MainCPUSelfResetRequest,      convertDefault);
        CONVERT(param, result,                error_counter_PowerBoardSEL,      convertDefault);
        CONVERT(param, result,                  error_counter_PowerCPUSEU,      convertDefault);
        CONVERT(param, result,              error_counter_MissionBoardSEL,      convertDefault);
        CONVERT(param, result, error_counter_MissionCPUShutdownNoResponse,      convertDefault);
        CONVERT(param, result,                error_counter_RadioBoardSEL,      convertDefault);
        CONVERT(param, result,         message_counter_SCI0CommandReceive,      convertDefault);
        CONVERT(param, result,            message_counter_SCI0CommandSend,      convertDefault);
        CONVERT(param, result,         message_counter_SCI2CommandReceive,      convertDefault);
        CONVERT(param, result,            message_counter_SCI2CommandSend,      convertDefault);
        CONVERT(param, result,           message_counter_RXCommandReceive,      convertDefault);
        CONVERT(param, result,              message_counter_TXCommandSend,      convertDefault);
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public static */TGSError TGSTelemetryINVADER::convert(std::string const& param, TelemetryRec* result)
{
    unsigned char binary[sizeof(BinaryRec)];
    int value;
    int i;
    int j;
    TGSError error(TGSERROR_OK);
    
    if (param.size() == sizeof(binary) * 2) {
        for (i = 0; i < sizeof(binary); ++i) {
            binary[i] = 0;
            for (j = 0; j < 2; ++j) {
                if ((value = toHexadecimal(param[i * 2 + j])) >= 0) {
                    binary[i] = (binary[i] << 4) | value;
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                    break;
                }
            }
            if (error != TGSERROR_OK) {
                break;
            }
        }
        if (error == TGSERROR_OK) {
            error = convert(*reinterpret_cast<BinaryRec*>(binary), result);
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*private static */void TGSTelemetryINVADER::initialize(void)
{
    TelemetryRec minimum;
    TelemetryRec maximum;
    
    if (convert(g_binaryMinimum, &minimum) == TGSERROR_OK) {
        if (convert(g_binaryMaximum, &maximum) == TGSERROR_OK) {
            g_telemetryMinimum = minimum;
            g_telemetryMaximum = maximum;
            NORMALIZE(                           obctime_OBCTime);
            NORMALIZE(                           voltage_Battery);
            NORMALIZE(                               voltage_Bus);
            NORMALIZE(                             voltage_Solar);
            NORMALIZE(                      ampare_PowerCPUBoard);
            NORMALIZE(                                ampare_Bus);
            NORMALIZE(                            ampare_Battery);
            NORMALIZE(                              ampare_Solar);
            NORMALIZE(                           ampare_SolarMY2);
            NORMALIZE(                           ampare_SolarPY2);
            NORMALIZE(                            ampare_SolarMZ);
            NORMALIZE(                            ampare_SolarPZ);
            NORMALIZE(                           ampare_SolarMY1);
            NORMALIZE(                           ampare_SolarPY1);
            NORMALIZE(                            ampare_SolarMX);
            NORMALIZE(                            ampare_SolarPX);
            NORMALIZE(                            ampare_Antenna);
            NORMALIZE(                             ampare_Heater);
            NORMALIZE(                                 ampare_TX);
            NORMALIZE(                                 ampare_CW);
            NORMALIZE(                                 ampare_RX);
            NORMALIZE(                            ampare_MainCPU);
            NORMALIZE(                         ampare_MissionCPU);
            NORMALIZE(                      temperature_Battery1);
            NORMALIZE(                      temperature_Battery2);
            NORMALIZE(                      temperature_Battery3);
            NORMALIZE(                      temperature_SolarMX1);
            NORMALIZE(                      temperature_SolarMX2);
            NORMALIZE(                      temperature_SolarPY1);
            NORMALIZE(                      temperature_SolarPY2);
            NORMALIZE(                      temperature_SolarMY1);
            NORMALIZE(                      temperature_SolarMY2);
            NORMALIZE(                       temperature_SolarPX);
            NORMALIZE(                      temperature_SolarPZ2);
            NORMALIZE(                      temperature_SolarMZ1);
            NORMALIZE(                      temperature_SolarMZ2);
            NORMALIZE(                 temperature_PowerCPUBoard);
            NORMALIZE(                       temperature_MainCPU);
            NORMALIZE(                          temperature_TXCW);
            NORMALIZE(                            temperature_RX);
            NORMALIZE(                  temperature_MainCPUBoard);
            NORMALIZE(                                    gyro_X);
            NORMALIZE(                                    gyro_Y);
            NORMALIZE(                                    gyro_Z);
            NORMALIZE(                                  magnet_X);
            NORMALIZE(                                  magnet_Y);
            NORMALIZE(                                  magnet_Z);
            NORMALIZE(                                 rssi_RSSI);
            NORMALIZE(    mission_status_UserReprogramModeEnable);
            NORMALIZE(              mission_status_BootProgramID);
            NORMALIZE(   mission_status_MissionCPUONIntervalTime);
            NORMALIZE(      mission_status_MissionCPUONOFFPeriod);
            NORMALIZE(      mission_status_MissionCPUONDutyCycle);
            NORMALIZE(     mission_status_ScheduledBootProgramID);
            NORMALIZE(          mission_status_ScheduledBootTime);
            NORMALIZE(   mission_status_MissionCPUReprogramCount);
            NORMALIZE(     mission_status_MissionCPUStartupCount);
            NORMALIZE(    mission_status_MissionCPUShutdownCount);
            NORMALIZE(   mission_status_SharedMemoryWritePointer);
            NORMALIZE(       init_status_MainCPUInternalFunction);
            NORMALIZE(       init_status_MainCPUInternalRegister);
            NORMALIZE(       init_status_MainCPUPeripheralDevice);
            NORMALIZE(                              mode_MainCPU);
            NORMALIZE(                      chargeflag_ChargeICR);
            NORMALIZE(                      chargeflag_ChargeICG);
            NORMALIZE(                                flag_Flags);
            NORMALIZE(                error_counter_MainBoardSEL);
            NORMALIZE(                  error_counter_MainCPUSEU);
            NORMALIZE(     error_counter_MainCPUSelfResetRequest);
            NORMALIZE(               error_counter_PowerBoardSEL);
            NORMALIZE(                 error_counter_PowerCPUSEU);
            NORMALIZE(             error_counter_MissionBoardSEL);
            NORMALIZE(error_counter_MissionCPUShutdownNoResponse);
            NORMALIZE(               error_counter_RadioBoardSEL);
            NORMALIZE(        message_counter_SCI0CommandReceive);
            NORMALIZE(           message_counter_SCI0CommandSend);
            NORMALIZE(        message_counter_SCI2CommandReceive);
            NORMALIZE(           message_counter_SCI2CommandSend);
            NORMALIZE(          message_counter_RXCommandReceive);
            NORMALIZE(             message_counter_TXCommandSend);
        }
    }
    return;
}

template <typename T>
/*private static */void TGSTelemetryINVADER::normalize(ValueRec<T> TelemetryRec::* field)
{
    T temp;
    
    if ((g_telemetryMinimum.*field).value > (g_telemetryMaximum.*field).value) {
        temp = (g_telemetryMinimum.*field).value;
        (g_telemetryMinimum.*field).value = (g_telemetryMaximum.*field).value;
        (g_telemetryMaximum.*field).value = temp;
    }
    return;
}

template <typename R, typename P>
/*private static */void TGSTelemetryINVADER::convert(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<R>* result, R (*function)(P))
{
    result->name = name;
    result->valid = true;
    if (binary.*field < g_binaryMinimum.*field || g_binaryMaximum.*field < binary.*field) {
        result->valid = false;
    }
    result->value = (*function)(binary.*field);
    return;
}

template <size_t N, typename R, typename P>
/*private static */void TGSTelemetryINVADER::convert(BinaryRec const& binary, unsigned char const (BinaryRec::* field)[N], char const* name, ValueRec<R>* result, R (*function)(P))
{
    P value;
    int i;
    
    result->name = name;
    result->valid = true;
    value = 0;
    for (i = 0; i < sizeof(binary.*field); ++i) {
        if ((binary.*field)[i] < (g_binaryMinimum.*field)[i] || (g_binaryMaximum.*field)[i] < (binary.*field)[i]) {
            result->valid = false;
        }
        value = (value << 8) | (binary.*field)[i];
    }
    result->value = (*function)(value);
    return;
}

/*private static */int TGSTelemetryINVADER::toHexadecimal(char param)
{
    int result(-1);
    
    if ('0' <= param && param <= '9') {
        result = param - '0';
    }
    else if ('a' <= param && param <= 'f') {
        result = param - 'a' + 10;
    }
    else if ('A' <= param && param <= 'F') {
        result = param - 'A' + 10;
    }
    return result;
}

}// end of namespace
