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
**      ASDServerDatabase.cpp
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

#include "ASDServerDatabase.h"
#include "writer.h"
#include "stringbuffer.h"
#include "artsatd.h"

#define DATA(doc, add, value, field)            (append((doc), (add), (value).field))
#define META(doc, add, minimum, maximum, field) (append((doc), (add), (minimum).field, (maximum).field))

/*public */ASDServerDatabase::ASDServerDatabase(void)
{
    _minimum = tgs::TGSTelemetryINVADER::getMinimum();
    _maximum = tgs::TGSTelemetryINVADER::getMaximum();
}

/*public virtual */ASDServerDatabase::~ASDServerDatabase(void)
{
}

/*private virtual */tgs::TGSError ASDServerDatabase::onRequest(RequestRec const& request, ResponseRec* response)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (request.path == "/data/simulator") {
        if ((error = responseData(&response->content)) == tgs::TGSERROR_OK) {
            response->status = 200;
        }
    }
    else if (request.path == "/meta") {
        if ((error = responseMeta(&response->content)) == tgs::TGSERROR_OK) {
            response->status = 200;
        }
    }
    else {
        response->status = 404;
    }
    return error;
}

/*private */tgs::TGSError ASDServerDatabase::responseData(std::string* response)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    rapidjson::Value history(rapidjson::kObjectType);
    rapidjson::Value telemetry(rapidjson::kObjectType);
    tgs::TGSTelemetryINVADER::TelemetryRec value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    doc.SetArray();
    history.AddMember("time", ir::IRXTime::currentUTCTime().formatISO8601().c_str(), doc.GetAllocator());
    // TODO:
    std::string data = "163C6E5903034600000008090039000102002600000000002A0028140700181F1A4B722F752E6F3372271E1C5C6F69390000C3A36DC1FDFD00000000000000000000000000000000000000000000000000000000000000000000000E010300000000000002040000003EC5C7F2C52B9715A900000000000000000000000000";
    // :ODOT
    if (tgs::TGSTelemetryINVADER::convert(data, &value) == tgs::TGSERROR_OK) {
        DATA(doc, &telemetry, value,                            obctime_OBCTime);
        DATA(doc, &telemetry, value,                            voltage_Battery);
        DATA(doc, &telemetry, value,                                voltage_Bus);
        DATA(doc, &telemetry, value,                              voltage_Solar);
        DATA(doc, &telemetry, value,                       ampare_PowerCPUBoard);
        DATA(doc, &telemetry, value,                                 ampare_Bus);
        DATA(doc, &telemetry, value,                             ampare_Battery);
        DATA(doc, &telemetry, value,                               ampare_Solar);
        DATA(doc, &telemetry, value,                            ampare_SolarMY2);
        DATA(doc, &telemetry, value,                            ampare_SolarPY2);
        DATA(doc, &telemetry, value,                             ampare_SolarMZ);
        DATA(doc, &telemetry, value,                             ampare_SolarPZ);
        DATA(doc, &telemetry, value,                            ampare_SolarMY1);
        DATA(doc, &telemetry, value,                            ampare_SolarPY1);
        DATA(doc, &telemetry, value,                             ampare_SolarMX);
        DATA(doc, &telemetry, value,                             ampare_SolarPX);
        DATA(doc, &telemetry, value,                             ampare_Antenna);
        DATA(doc, &telemetry, value,                              ampare_Heater);
        DATA(doc, &telemetry, value,                                  ampare_TX);
        DATA(doc, &telemetry, value,                                  ampare_CW);
        DATA(doc, &telemetry, value,                                  ampare_RX);
        DATA(doc, &telemetry, value,                             ampare_MainCPU);
        DATA(doc, &telemetry, value,                          ampare_MissionCPU);
        DATA(doc, &telemetry, value,                       temperature_Battery1);
        DATA(doc, &telemetry, value,                       temperature_Battery2);
        DATA(doc, &telemetry, value,                       temperature_Battery3);
        DATA(doc, &telemetry, value,                       temperature_SolarMX1);
        DATA(doc, &telemetry, value,                       temperature_SolarMX2);
        DATA(doc, &telemetry, value,                       temperature_SolarPY1);
        DATA(doc, &telemetry, value,                       temperature_SolarPY2);
        DATA(doc, &telemetry, value,                       temperature_SolarMY1);
        DATA(doc, &telemetry, value,                       temperature_SolarMY2);
        DATA(doc, &telemetry, value,                        temperature_SolarPX);
        DATA(doc, &telemetry, value,                       temperature_SolarPZ2);
        DATA(doc, &telemetry, value,                       temperature_SolarMZ1);
        DATA(doc, &telemetry, value,                       temperature_SolarMZ2);
        DATA(doc, &telemetry, value,                  temperature_PowerCPUBoard);
        DATA(doc, &telemetry, value,                        temperature_MainCPU);
        DATA(doc, &telemetry, value,                           temperature_TXCW);
        DATA(doc, &telemetry, value,                             temperature_RX);
        DATA(doc, &telemetry, value,                   temperature_MainCPUBoard);
        DATA(doc, &telemetry, value,                                     gyro_X);
        DATA(doc, &telemetry, value,                                     gyro_Y);
        DATA(doc, &telemetry, value,                                     gyro_Z);
        DATA(doc, &telemetry, value,                                   magnet_X);
        DATA(doc, &telemetry, value,                                   magnet_Y);
        DATA(doc, &telemetry, value,                                   magnet_Z);
        DATA(doc, &telemetry, value,                                  rssi_RSSI);
        DATA(doc, &telemetry, value,     mission_status_UserReprogramModeEnable);
        DATA(doc, &telemetry, value,               mission_status_BootProgramID);
        DATA(doc, &telemetry, value,    mission_status_MissionCPUONIntervalTime);
        DATA(doc, &telemetry, value,       mission_status_MissionCPUONOFFPeriod);
        DATA(doc, &telemetry, value,       mission_status_MissionCPUONDutyCycle);
        DATA(doc, &telemetry, value,      mission_status_ScheduledBootProgramID);
        DATA(doc, &telemetry, value,           mission_status_ScheduledBootTime);
        DATA(doc, &telemetry, value,    mission_status_MissionCPUReprogramCount);
        DATA(doc, &telemetry, value,      mission_status_MissionCPUStartupCount);
        DATA(doc, &telemetry, value,     mission_status_MissionCPUShutdownCount);
        DATA(doc, &telemetry, value,    mission_status_SharedMemoryWritePointer);
        DATA(doc, &telemetry, value,        init_status_MainCPUInternalFunction);
        DATA(doc, &telemetry, value,        init_status_MainCPUInternalRegister);
        DATA(doc, &telemetry, value,        init_status_MainCPUPeripheralDevice);
        DATA(doc, &telemetry, value,                               mode_MainCPU);
        DATA(doc, &telemetry, value,                       chargeflag_ChargeICR);
        DATA(doc, &telemetry, value,                       chargeflag_ChargeICG);
        DATA(doc, &telemetry, value,                                 flag_Flags);
        DATA(doc, &telemetry, value,                 error_counter_MainBoardSEL);
        DATA(doc, &telemetry, value,                   error_counter_MainCPUSEU);
        DATA(doc, &telemetry, value,      error_counter_MainCPUSelfResetRequest);
        DATA(doc, &telemetry, value,                error_counter_PowerBoardSEL);
        DATA(doc, &telemetry, value,                  error_counter_PowerCPUSEU);
        DATA(doc, &telemetry, value,              error_counter_MissionBoardSEL);
        DATA(doc, &telemetry, value, error_counter_MissionCPUShutdownNoResponse);
        DATA(doc, &telemetry, value,                error_counter_RadioBoardSEL);
        DATA(doc, &telemetry, value,         message_counter_SCI0CommandReceive);
        DATA(doc, &telemetry, value,            message_counter_SCI0CommandSend);
        DATA(doc, &telemetry, value,         message_counter_SCI2CommandReceive);
        DATA(doc, &telemetry, value,            message_counter_SCI2CommandSend);
        DATA(doc, &telemetry, value,           message_counter_RXCommandReceive);
        DATA(doc, &telemetry, value,              message_counter_TXCommandSend);
    }
    history.AddMember("telemetry", telemetry, doc.GetAllocator());
    doc.PushBack(history, doc.GetAllocator());
    doc.Accept(writer);
    *response = buffer.GetString();
    return error;
}

/*private */tgs::TGSError ASDServerDatabase::responseMeta(std::string* response)
{
    rapidjson::Document doc;
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    rapidjson::Value telemetry(rapidjson::kObjectType);
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    doc.SetObject();
    doc.AddMember("time", ir::IRXTime::currentUTCTime().formatISO8601().c_str(), doc.GetAllocator());
    META(doc, &telemetry, _minimum, _maximum,                            obctime_OBCTime);
    META(doc, &telemetry, _minimum, _maximum,                            voltage_Battery);
    META(doc, &telemetry, _minimum, _maximum,                                voltage_Bus);
    META(doc, &telemetry, _minimum, _maximum,                              voltage_Solar);
    META(doc, &telemetry, _minimum, _maximum,                       ampare_PowerCPUBoard);
    META(doc, &telemetry, _minimum, _maximum,                                 ampare_Bus);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_Battery);
    META(doc, &telemetry, _minimum, _maximum,                               ampare_Solar);
    META(doc, &telemetry, _minimum, _maximum,                            ampare_SolarMY2);
    META(doc, &telemetry, _minimum, _maximum,                            ampare_SolarPY2);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_SolarMZ);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_SolarPZ);
    META(doc, &telemetry, _minimum, _maximum,                            ampare_SolarMY1);
    META(doc, &telemetry, _minimum, _maximum,                            ampare_SolarPY1);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_SolarMX);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_SolarPX);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_Antenna);
    META(doc, &telemetry, _minimum, _maximum,                              ampare_Heater);
    META(doc, &telemetry, _minimum, _maximum,                                  ampare_TX);
    META(doc, &telemetry, _minimum, _maximum,                                  ampare_CW);
    META(doc, &telemetry, _minimum, _maximum,                                  ampare_RX);
    META(doc, &telemetry, _minimum, _maximum,                             ampare_MainCPU);
    META(doc, &telemetry, _minimum, _maximum,                          ampare_MissionCPU);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_Battery1);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_Battery2);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_Battery3);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMX1);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMX2);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarPY1);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarPY2);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMY1);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMY2);
    META(doc, &telemetry, _minimum, _maximum,                        temperature_SolarPX);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarPZ2);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMZ1);
    META(doc, &telemetry, _minimum, _maximum,                       temperature_SolarMZ2);
    META(doc, &telemetry, _minimum, _maximum,                  temperature_PowerCPUBoard);
    META(doc, &telemetry, _minimum, _maximum,                        temperature_MainCPU);
    META(doc, &telemetry, _minimum, _maximum,                           temperature_TXCW);
    META(doc, &telemetry, _minimum, _maximum,                             temperature_RX);
    META(doc, &telemetry, _minimum, _maximum,                   temperature_MainCPUBoard);
    META(doc, &telemetry, _minimum, _maximum,                                     gyro_X);
    META(doc, &telemetry, _minimum, _maximum,                                     gyro_Y);
    META(doc, &telemetry, _minimum, _maximum,                                     gyro_Z);
    META(doc, &telemetry, _minimum, _maximum,                                   magnet_X);
    META(doc, &telemetry, _minimum, _maximum,                                   magnet_Y);
    META(doc, &telemetry, _minimum, _maximum,                                   magnet_Z);
    META(doc, &telemetry, _minimum, _maximum,                                  rssi_RSSI);
    META(doc, &telemetry, _minimum, _maximum,     mission_status_UserReprogramModeEnable);
    META(doc, &telemetry, _minimum, _maximum,               mission_status_BootProgramID);
    META(doc, &telemetry, _minimum, _maximum,    mission_status_MissionCPUONIntervalTime);
    META(doc, &telemetry, _minimum, _maximum,       mission_status_MissionCPUONOFFPeriod);
    META(doc, &telemetry, _minimum, _maximum,       mission_status_MissionCPUONDutyCycle);
    META(doc, &telemetry, _minimum, _maximum,      mission_status_ScheduledBootProgramID);
    META(doc, &telemetry, _minimum, _maximum,           mission_status_ScheduledBootTime);
    META(doc, &telemetry, _minimum, _maximum,    mission_status_MissionCPUReprogramCount);
    META(doc, &telemetry, _minimum, _maximum,      mission_status_MissionCPUStartupCount);
    META(doc, &telemetry, _minimum, _maximum,     mission_status_MissionCPUShutdownCount);
    META(doc, &telemetry, _minimum, _maximum,    mission_status_SharedMemoryWritePointer);
    META(doc, &telemetry, _minimum, _maximum,        init_status_MainCPUInternalFunction);
    META(doc, &telemetry, _minimum, _maximum,        init_status_MainCPUInternalRegister);
    META(doc, &telemetry, _minimum, _maximum,        init_status_MainCPUPeripheralDevice);
    META(doc, &telemetry, _minimum, _maximum,                               mode_MainCPU);
    META(doc, &telemetry, _minimum, _maximum,                       chargeflag_ChargeICR);
    META(doc, &telemetry, _minimum, _maximum,                       chargeflag_ChargeICG);
    META(doc, &telemetry, _minimum, _maximum,                                 flag_Flags);
    META(doc, &telemetry, _minimum, _maximum,                 error_counter_MainBoardSEL);
    META(doc, &telemetry, _minimum, _maximum,                   error_counter_MainCPUSEU);
    META(doc, &telemetry, _minimum, _maximum,      error_counter_MainCPUSelfResetRequest);
    META(doc, &telemetry, _minimum, _maximum,                error_counter_PowerBoardSEL);
    META(doc, &telemetry, _minimum, _maximum,                  error_counter_PowerCPUSEU);
    META(doc, &telemetry, _minimum, _maximum,              error_counter_MissionBoardSEL);
    META(doc, &telemetry, _minimum, _maximum, error_counter_MissionCPUShutdownNoResponse);
    META(doc, &telemetry, _minimum, _maximum,                error_counter_RadioBoardSEL);
    META(doc, &telemetry, _minimum, _maximum,         message_counter_SCI0CommandReceive);
    META(doc, &telemetry, _minimum, _maximum,            message_counter_SCI0CommandSend);
    META(doc, &telemetry, _minimum, _maximum,         message_counter_SCI2CommandReceive);
    META(doc, &telemetry, _minimum, _maximum,            message_counter_SCI2CommandSend);
    META(doc, &telemetry, _minimum, _maximum,           message_counter_RXCommandReceive);
    META(doc, &telemetry, _minimum, _maximum,              message_counter_TXCommandSend);
    doc.AddMember("telemetry", telemetry, doc.GetAllocator());
    doc.Accept(writer);
    *response = buffer.GetString();
    return error;
}

template <typename T>
/*private static */void ASDServerDatabase::append(rapidjson::Document& doc, rapidjson::Value* object, tgs::TGSTelemetryINVADER::ValueRec<T> const& value)
{
    rapidjson::Value array(rapidjson::kArrayType);
    
    array.PushBack((value.valid) ? (rapidjson::kTrueType) : (rapidjson::kFalseType), doc.GetAllocator());
    array.PushBack(value.value, doc.GetAllocator());
    object->AddMember(value.name, doc.GetAllocator(), array, doc.GetAllocator());
    return;
}

template <typename T>
/*private static */void ASDServerDatabase::append(rapidjson::Document& doc, rapidjson::Value* object, tgs::TGSTelemetryINVADER::ValueRec<T> const& minimum, tgs::TGSTelemetryINVADER::ValueRec<T> const& maximum)
{
    rapidjson::Value array(rapidjson::kArrayType);
    
    array.PushBack(minimum.value, doc.GetAllocator());
    array.PushBack(maximum.value, doc.GetAllocator());
    object->AddMember(minimum.name, doc.GetAllocator(), array, doc.GetAllocator());
    return;
}
