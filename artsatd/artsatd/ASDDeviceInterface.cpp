/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2015 HORIGUCHI Junshi.
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
**      ASDDeviceInterface.cpp
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

#include "ASDDeviceInterface.h"
#include "artsatd.h"

#define UPDATE_INTERVAL                         (33)
#define SYNCHRONIZE_DIVISION                    (6)

template <class T>
/*protected */ASDDeviceInterface<T>::ASDDeviceInterface(void)
{
}

template <class T>
/*public virtual */ASDDeviceInterface<T>::~ASDDeviceInterface(void)
{
    close();
}

template <class T>
/*public */bool ASDDeviceInterface<T>::isValid(void) const
{
    bool result(false);
    
    if (_device != NULL) {
        result = _device->isValid();
    }
    return result;
}

template <class T>
/*public */tgs::TGSError ASDDeviceInterface<T>::open(boost::shared_ptr<tgs::TGSDeviceInterface> const& device)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    if (device != NULL) {
        _device = device;
        update();
        synchronize();
        try {
            _thread = boost::thread(boost::bind(&ASDDeviceInterface::thread, this));
        }
        catch (boost::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread start error [%s]", boost::diagnostic_information(e).c_str());
            error = tgs::TGSERROR_FAILED;
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread start error [%s]", e.what());
            error = tgs::TGSERROR_FAILED;
        }
        catch (...) {
            artsatd::getInstance().log(LOG_EMERG, "thread start error [...]");
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

template <class T>
/*public */void ASDDeviceInterface<T>::close(void)
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

template <class T>
/*protected virtual */void ASDDeviceInterface<T>::update(ptr_type device)
{
    return;
}

template <class T>
/*private */void ASDDeviceInterface<T>::thread(void)
{
    int count;
    
    count = 0;
    while (!boost::this_thread::interruption_requested()) {
        try {
            boost::this_thread::sleep(boost::posix_time::milliseconds(UPDATE_INTERVAL));
        }
        catch (boost::thread_interrupted& e) {
            break;
        }
        catch (boost::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread sleep error [%s]", boost::diagnostic_information(e).c_str());
            break;
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread sleep error [%s]", e.what());
            break;
        }
        catch (...) {
            artsatd::getInstance().log(LOG_EMERG, "thread sleep error [...]");
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

template <class T>
/*private */void ASDDeviceInterface<T>::update(void)
{
    if (_device->isValid()) {
        boost::unique_lock<boost::mutex> lock(_mutex);
        _device->update();
    }
    return;
}

template <class T>
/*private */void ASDDeviceInterface<T>::synchronize(void)
{
    if (_device->isValid()) {
        boost::unique_lock<boost::mutex> lock(_mutex);
        update(boost::static_pointer_cast<T>(_device).get());
    }
    return;
}

template class ASDDeviceInterface<tgs::TGSRotatorInterface>;
template class ASDDeviceInterface<tgs::TGSTransceiverInterface>;
template class ASDDeviceInterface<tgs::TGSTNCInterface>;
