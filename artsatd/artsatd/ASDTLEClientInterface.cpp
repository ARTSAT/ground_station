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
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      ASDTLEClientInterface.cpp
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

#include "ASDTLEClientInterface.h"
#include "artsatd.h"

/*protected */ASDTLEClientInterface::ASDTLEClientInterface(void)
{
}

/*public virtual */ASDTLEClientInterface::~ASDTLEClientInterface(void)
{
    close();
}

/*public */tgs::TGSError ASDTLEClientInterface::open(std::string const& file, std::vector<std::string> const& url, int interval)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    if (interval > 0) {
        if ((error = _database.open(file)) == tgs::TGSERROR_OK) {
            _url = url;
            _interval = interval;
            update();
            try {
                _thread = boost::thread(boost::bind(&ASDTLEClientInterface::thread, this));
            }
            catch (std::exception& e) {
                artsatd::getInstance().log(LOG_EMERG, "thread start error [%s]", e.what());
                error = tgs::TGSERROR_FAILED;
            }
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

/*public */void ASDTLEClientInterface::close(void)
{
    _thread.interrupt();
    try {
        _thread.join();
    }
    catch (...) {
    }
    _database.close();
    return;
}

/*protected virtual */tgs::TGSError ASDTLEClientInterface::parse(std::string const& content, tgs::TGSPhysicsDatabase* database)
{
    return tgs::TGSERROR_NO_SUPPORT;
}

/*private */void ASDTLEClientInterface::thread(void)
{
    while (!boost::this_thread::interruption_requested()) {
        try {
            boost::this_thread::sleep(boost::posix_time::milliseconds(_interval));
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "thread sleep error [%s]", e.what());
            break;
        }
        update();
    }
    return;
}

/*private */void ASDTLEClientInterface::update(void)
{
    std::vector<std::string>::const_iterator it;
    boost::network::http::client client(boost::network::http::client::options().follow_redirects(true).cache_resolved(true));
    boost::network::http::client::request request;
    boost::network::http::client::response response;
    int code;
    tgs::TGSError error;
    
    for (it = _url.begin(); it != _url.end(); ++it) {
        if (!it->empty()) {
            try {
                request.uri(*it);
                response = client.get(request);
                code = status(response);
                switch (code) {
                    case 200:
                        if ((error = _database.begin()) == tgs::TGSERROR_OK) {
                            error = parse(body(response), &_database);
                            _database.end();
                        }
                        if (error != tgs::TGSERROR_OK) {
                            artsatd::getInstance().log(LOG_ERR, "tle client parse error [%s] %s", error.print().c_str(), it->c_str());
                        }
                        break;
                    default:
                        artsatd::getInstance().log(LOG_ERR, "tle client status error [%d] %s", code, it->c_str());
                        break;
                }
            }
            catch (std::exception& e) {
                artsatd::getInstance().log(LOG_ERR, "tle client connection error [%s] %s", e.what(), it->c_str());
            }
        }
    }
    return;
}
