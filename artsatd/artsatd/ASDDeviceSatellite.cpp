/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++03
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      ASDDeviceSatellite.cpp
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

#include "ASDDeviceSatellite.h"
#include "artsatd.h"

#define UPDATE_INTERVAL                         (33)
#define SYNCHRONIZE_DIVISION                    (6)

/*public */ASDDeviceSatellite::DataRec ASDDeviceSatellite::getData(void) const
{
    boost::lock_guard<boost::mutex> guard(_mutex_data);
    
    _update = false;
    return _data;
}

/*public */bool ASDDeviceSatellite::hasUpdate(void) const
{
    bool result(false);
    
    if (_device != NULL) {
        _mutex_data.lock();
        result = _update;
        _mutex_data.unlock();
    }
    return result;
}

/*public */tgs::TGSError ASDDeviceSatellite::open(boost::shared_ptr<tgs::TGSSatelliteInterface> const& device)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    if (device != NULL) {
        _device = device;
        _data.status = "";
        _update = false;
        update();
        synchronize();
        try {
            _thread = boost::thread(boost::bind(&ASDDeviceSatellite::thread, this));
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread start error [%s]", e.what());
            error = tgs::TGSERROR_FAILED;
        }
        if (error != tgs::TGSERROR_OK) {
            close();
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */void ASDDeviceSatellite::close(void)
{
    _thread.interrupt();
    try {
        _thread.join();
    }
    catch (...) {
    }
    _device.reset();
    return;
}

/*private */void ASDDeviceSatellite::thread(void)
{
    int count;
    
    count = 0;
    while (!boost::this_thread::interruption_requested()) {
        try {
            boost::this_thread::sleep(boost::posix_time::milliseconds(UPDATE_INTERVAL));
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread sleep error [%s]", e.what());
            break;
        }
        update();
        if (++count >= SYNCHRONIZE_DIVISION) {
            synchronize();
            count = 0;
        }
    }
    return;
}

/*private */void ASDDeviceSatellite::update(void)
{
    _mutex_device.lock();
    _device->update();
    _mutex_device.unlock();
    return;
}

/*private */void ASDDeviceSatellite::synchronize(void)
{
    bool valid;
    DataRec data;
    tgs::TGSError error;
    
    valid = false;
    _mutex_device.lock();
    if (_device->isValid()) {
        valid = true;
        data = _data;
        if ((error = _device->getStatus(&data.status)) != tgs::TGSERROR_OK) {
            artsatd::getInstance().log(LOG_ERR, "TGSSatelliteInterface getStatus error [%s]", error.print().c_str());
        }
    }
    _mutex_device.unlock();
    if (valid) {
        _mutex_data.lock();
        _data = data;
        _update = true;
        _mutex_data.unlock();
    }
    return;
}
