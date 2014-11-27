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
**      TGSTelemetryINVADER.h
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

#ifndef __TGS_TELEMETRYINVADER_H
#define __TGS_TELEMETRYINVADER_H

#include "TGSType.h"
#include <math.h>

namespace tgs {

class TGSTelemetryINVADER {
    public:
        enum FlagEnum {
            FLAG_ADCONVERT                  = 1 << 0,
            FLAG_SUMMARYHISTORYWRITE        = 1 << 1,
            FLAG_DETAILHISTORYWRITE         = 1 << 2,
            FLAG_APIDATAWRITE               = 1 << 3,
            FLAG_SERIALBUFFERREAD           = 1 << 4,
            FLAG_RXBUFFERREAD               = 1 << 5,
            FLAG_POWERINFOREQUEST           = 1 << 6,
            FLAG_SENSORDATATRANSFER         = 1 << 7
        };
    
    public:
        struct BinaryRec {
            unsigned char                   obctime_OBCTime[4];
            unsigned char                   voltage_Battery;
            unsigned char                   voltage_Bus;
            unsigned char                   voltage_Solar;
            unsigned char                   reserved_008;
            unsigned char                   reserved_009;
            unsigned char                   reserved_010;
            unsigned char                   ampare_PowerCPUBoard;
            unsigned char                   ampare_Bus;
            unsigned char                   ampare_Battery;
            unsigned char                   ampare_Solar;
            unsigned char                   ampare_SolarMY2;
            unsigned char                   ampare_SolarPY2;
            unsigned char                   ampare_SolarMZ;
            unsigned char                   ampare_SolarPZ;
            unsigned char                   ampare_SolarMY1;
            unsigned char                   ampare_SolarPY1;
            unsigned char                   ampare_SolarMX;
            unsigned char                   ampare_SolarPX;
            unsigned char                   ampare_Antenna;
            unsigned char                   ampare_Heater;
            unsigned char                   ampare_TX;
            unsigned char                   ampare_CW;
            unsigned char                   ampare_RX;
            unsigned char                   ampare_MainCPU;
            unsigned char                   ampare_MissionCPU;
            unsigned char                   reserved_030;
            unsigned char                   temperature_Battery1;
            unsigned char                   temperature_Battery2;
            unsigned char                   temperature_Battery3;
            unsigned char                   temperature_SolarMX1;
            unsigned char                   temperature_SolarMX2;
            unsigned char                   temperature_SolarPY1;
            unsigned char                   temperature_SolarPY2;
            unsigned char                   temperature_SolarMY1;
            unsigned char                   temperature_SolarMY2;
            unsigned char                   temperature_SolarPX;
            unsigned char                   temperature_SolarPZ2;
            unsigned char                   temperature_SolarMZ1;
            unsigned char                   temperature_SolarMZ2;
            unsigned char                   temperature_PowerCPUBoard;
            unsigned char                   temperature_MainCPU;
            unsigned char                   temperature_TXCW;
            unsigned char                   temperature_RX;
            unsigned char                   temperature_MainCPUBoard;
            unsigned char                   reserved_049;
            unsigned char                   reserved_050;
            unsigned char                   gyro_X;
            unsigned char                   gyro_Y;
            unsigned char                   gyro_Z;
            unsigned char                   magnet_X;
            unsigned char                   magnet_Y;
            unsigned char                   magnet_Z;
            unsigned char                   reserved_057;
            unsigned char                   reserved_058;
            unsigned char                   reserved_059;
            unsigned char                   rssi_RSSI;
            unsigned char                   reserved_061;
            unsigned char                   mission_status_UserReprogramModeEnable;
            unsigned char                   mission_status_BootProgramID;
            unsigned char                   mission_status_MissionCPUONIntervalTime[2];
            unsigned char                   mission_status_MissionCPUONOFFPeriod[2];
            unsigned char                   mission_status_MissionCPUONDutyCycle;
            unsigned char                   mission_status_ScheduledBootProgramID;
            unsigned char                   mission_status_ScheduledBootTime[2];
            unsigned char                   mission_status_MissionCPUReprogramCount;
            unsigned char                   mission_status_MissionCPUStartupCount;
            unsigned char                   mission_status_MissionCPUShutdownCount;
            unsigned char                   mission_status_SharedMemoryWritePointer[3];
            unsigned char                   reserved_078;
            unsigned char                   reserved_079;
            unsigned char                   reserved_080;
            unsigned char                   power_status_reserved[8];
            unsigned char                   reserved_089;
            unsigned char                   reserved_090;
            unsigned char                   reserved_091;
            unsigned char                   init_status_MainCPUInternalFunction[2];
            unsigned char                   init_status_MainCPUInternalRegister[2];
            unsigned char                   init_status_MainCPUPeripheralDevice[2];
            unsigned char                   reserved_098;
            unsigned char                   reserved_099;
            unsigned char                   mode_MainCPU;
            unsigned char                   chargeflag_ChargeICR;
            unsigned char                   chargeflag_ChargeICG;
            unsigned char                   flag_Flags;
            unsigned char                   flag_reserved;
            unsigned char                   reserved_105;
            unsigned char                   error_counter_MainBoardSEL;
            unsigned char                   error_counter_MainCPUSEU;
            unsigned char                   error_counter_MainCPUSelfResetRequest;
            unsigned char                   error_counter_PowerBoardSEL;
            unsigned char                   error_counter_PowerCPUSEU;
            unsigned char                   error_counter_MissionBoardSEL;
            unsigned char                   error_counter_MissionCPUShutdownNoResponse;
            unsigned char                   error_counter_RadioBoardSEL;
            unsigned char                   reserved_114;
            unsigned char                   message_counter_SCI0CommandReceive[2];
            unsigned char                   message_counter_SCI0CommandSend[2];
            unsigned char                   message_counter_SCI2CommandReceive[2];
            unsigned char                   message_counter_SCI2CommandSend[2];
            unsigned char                   message_counter_RXCommandReceive[2];
            unsigned char                   message_counter_TXCommandSend[2];
            unsigned char                   reserved_127;
        };
        template <typename T>
        struct ValueRec {
            char const*                     name;
            bool                            valid;
            T                               value;
        };
        struct TelemetryRec {
            ValueRec<unsigned int>          obctime_OBCTime;
            ValueRec<int>                   voltage_Battery;
            ValueRec<int>                   voltage_Bus;
            ValueRec<int>                   voltage_Solar;
            ValueRec<double>                ampare_PowerCPUBoard;
            ValueRec<double>                ampare_Bus;
            ValueRec<double>                ampare_Battery;
            ValueRec<double>                ampare_Solar;
            ValueRec<double>                ampare_SolarMY2;
            ValueRec<double>                ampare_SolarPY2;
            ValueRec<double>                ampare_SolarMZ;
            ValueRec<double>                ampare_SolarPZ;
            ValueRec<double>                ampare_SolarMY1;
            ValueRec<double>                ampare_SolarPY1;
            ValueRec<double>                ampare_SolarMX;
            ValueRec<double>                ampare_SolarPX;
            ValueRec<double>                ampare_Antenna;
            ValueRec<double>                ampare_Heater;
            ValueRec<double>                ampare_TX;
            ValueRec<double>                ampare_CW;
            ValueRec<double>                ampare_RX;
            ValueRec<double>                ampare_MainCPU;
            ValueRec<double>                ampare_MissionCPU;
            ValueRec<double>                temperature_Battery1;
            ValueRec<double>                temperature_Battery2;
            ValueRec<double>                temperature_Battery3;
            ValueRec<double>                temperature_SolarMX1;
            ValueRec<double>                temperature_SolarMX2;
            ValueRec<double>                temperature_SolarPY1;
            ValueRec<double>                temperature_SolarPY2;
            ValueRec<double>                temperature_SolarMY1;
            ValueRec<double>                temperature_SolarMY2;
            ValueRec<double>                temperature_SolarPX;
            ValueRec<double>                temperature_SolarPZ2;
            ValueRec<double>                temperature_SolarMZ1;
            ValueRec<double>                temperature_SolarMZ2;
            ValueRec<double>                temperature_PowerCPUBoard;
            ValueRec<double>                temperature_MainCPU;
            ValueRec<double>                temperature_TXCW;
            ValueRec<double>                temperature_RX;
            ValueRec<double>                temperature_MainCPUBoard;
            ValueRec<double>                gyro_X;
            ValueRec<double>                gyro_Y;
            ValueRec<double>                gyro_Z;
            ValueRec<double>                magnet_X;
            ValueRec<double>                magnet_Y;
            ValueRec<double>                magnet_Z;
            ValueRec<int>                   rssi_RSSI;
            ValueRec<int>                   mission_status_UserReprogramModeEnable;
            ValueRec<int>                   mission_status_BootProgramID;
            ValueRec<int>                   mission_status_MissionCPUONIntervalTime;
            ValueRec<int>                   mission_status_MissionCPUONOFFPeriod;
            ValueRec<int>                   mission_status_MissionCPUONDutyCycle;
            ValueRec<int>                   mission_status_ScheduledBootProgramID;
            ValueRec<int>                   mission_status_ScheduledBootTime;
            ValueRec<int>                   mission_status_MissionCPUReprogramCount;
            ValueRec<int>                   mission_status_MissionCPUStartupCount;
            ValueRec<int>                   mission_status_MissionCPUShutdownCount;
            ValueRec<int>                   mission_status_SharedMemoryWritePointer;
            ValueRec<int>                   init_status_MainCPUInternalFunction;
            ValueRec<int>                   init_status_MainCPUInternalRegister;
            ValueRec<int>                   init_status_MainCPUPeripheralDevice;
            ValueRec<int>                   mode_MainCPU;
            ValueRec<unsigned char>         chargeflag_ChargeICR;
            ValueRec<unsigned char>         chargeflag_ChargeICG;
            ValueRec<unsigned char>         flag_Flags;
            ValueRec<int>                   error_counter_MainBoardSEL;
            ValueRec<int>                   error_counter_MainCPUSEU;
            ValueRec<int>                   error_counter_MainCPUSelfResetRequest;
            ValueRec<int>                   error_counter_PowerBoardSEL;
            ValueRec<int>                   error_counter_PowerCPUSEU;
            ValueRec<int>                   error_counter_MissionBoardSEL;
            ValueRec<int>                   error_counter_MissionCPUShutdownNoResponse;
            ValueRec<int>                   error_counter_RadioBoardSEL;
            ValueRec<int>                   message_counter_SCI0CommandReceive;
            ValueRec<int>                   message_counter_SCI0CommandSend;
            ValueRec<int>                   message_counter_SCI2CommandReceive;
            ValueRec<int>                   message_counter_SCI2CommandSend;
            ValueRec<int>                   message_counter_RXCommandReceive;
            ValueRec<int>                   message_counter_TXCommandSend;
        };
    
    public:
        static  TelemetryRec                getMinimum                      (void);
        static  TelemetryRec                getMaximum                      (void);
        static  TGSError                    convert                         (BinaryRec const& param, TelemetryRec* result);
        static  TGSError                    convert                         (std::string const& param, TelemetryRec* result);
    private:
        static  void                        initialize                      (void);
        template <typename T>
        static  void                        normalize                       (ValueRec<T> TelemetryRec::* field);
        template <typename R, typename P>
        static  void                        convert                         (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<R>* result, R (*function)(P));
        template <size_t N, typename R, typename P>
        static  void                        convert                         (BinaryRec const& binary, unsigned char const (BinaryRec::* field)[N], char const* name, ValueRec<R>* result, R (*function)(P));
        template <typename T>
        static  void                        convertDefault                  (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<T>* result);
        template <size_t N, typename T>
        static  void                        convertDefault                  (BinaryRec const& binary, unsigned char const (BinaryRec::* field)[N], char const* name, ValueRec<T>* result);
        static  void                        convertCurrent1                 (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result);
        static  void                        convertCurrent2                 (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result);
        static  void                        convertTemperature              (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result);
        static  void                        convertGyro                     (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result);
        static  void                        convertMagnetometer             (BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result);
        template <typename R, typename P>
        static  R                           toDefault                       (P param);
        template <typename P>
        static  double                      toCurrent1                      (P param);
        template <typename P>
        static  double                      toCurrent2                      (P param);
        template <typename P>
        static  double                      toTemperature                   (P param);
        template <typename P>
        static  double                      toGyro                          (P param);
        template <typename P>
        static  double                      toMagnetometer                  (P param);
        static  int                         toHexadecimal                   (char param);
    private:
        explicit                            TGSTelemetryINVADER             (void);
                                            TGSTelemetryINVADER             (TGSTelemetryINVADER const&);
                                            ~TGSTelemetryINVADER            (void);
                TGSTelemetryINVADER&        operator=                       (TGSTelemetryINVADER const&);
};

template <typename T>
/*private static */inline void TGSTelemetryINVADER::convertDefault(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<T>* result)
{
    convert(binary, field, name, result, &toDefault<T, unsigned char>);
    return;
}

template <size_t N, typename T>
/*private static */inline void TGSTelemetryINVADER::convertDefault(BinaryRec const& binary, unsigned char const (BinaryRec::* field)[N], char const* name, ValueRec<T>* result)
{
    convert(binary, field, name, result, &toDefault<T, unsigned long>);
    return;
}

/*private static */inline void TGSTelemetryINVADER::convertCurrent1(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result)
{
    convert(binary, field, name, result, &toCurrent1<unsigned char>);
    return;
}

/*private static */inline void TGSTelemetryINVADER::convertCurrent2(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result)
{
    convert(binary, field, name, result, &toCurrent2<unsigned char>);
    return;
}

/*private static */inline void TGSTelemetryINVADER::convertTemperature(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result)
{
    convert(binary, field, name, result, &toTemperature<unsigned char>);
    return;
}

/*private static */inline void TGSTelemetryINVADER::convertGyro(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result)
{
    convert(binary, field, name, result, &toGyro<unsigned char>);
    return;
}

/*private static */inline void TGSTelemetryINVADER::convertMagnetometer(BinaryRec const& binary, unsigned char const BinaryRec::* field, char const* name, ValueRec<double>* result)
{
    convert(binary, field, name, result, &toMagnetometer<unsigned char>);
    return;
}

template <typename R, typename P>
/*private static */inline R TGSTelemetryINVADER::toDefault(P param)
{
    return static_cast<R>(param);
}

template <typename P>
/*private static */inline double TGSTelemetryINVADER::toCurrent1(P param)
{
    return param / 255.0;
}

template <typename P>
/*private static */inline double TGSTelemetryINVADER::toCurrent2(P param)
{
    return param * 2.5 / 255.0;
}

template <typename P>
/*private static */inline double TGSTelemetryINVADER::toTemperature(P param)
{
    return -1481.96 + sqrt(2.1952e+6 + (1.8639 - ((param * 5.0 / 255.0 - 2.5) / 4.0 + 5.0 / 3.0)) / 3.88e-6);
}

template <typename P>
/*private static */inline double TGSTelemetryINVADER::toGyro(P param)
{
    return ((param * 5.0 / 255.0 - 2.5) / 51.0 + 1.65 - 1.1 * 1.5) / (1.1 * 1.2e-2);
}

template <typename P>
/*private static */inline double TGSTelemetryINVADER::toMagnetometer(P param)
{
    return ((param * 5.0 / 255.0 - 2.5) / 7.25 + 1.65 - 1.1 * 1.5) / (1.1 * 2.4e-6);
}

}// end of namespace

#endif
