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
**      ASDPluginInterface.cpp
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

#include "ASDPluginInterface.h"
#include "artsatd.h"

/*protected */ASDPluginInterface::ASDPluginInterface(void)
{
}

/*public virtual */ASDPluginInterface::~ASDPluginInterface(void)
{
    close();
}

/*public virtual */tgs::TGSError ASDPluginInterface::open(std::string const& file)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!file.empty()) {
        error = serializeCache(file, &_skeleton);
    }
    if (error != tgs::TGSERROR_OK) {
        self::close();
    }
    return error;
}

/*public virtual */void ASDPluginInterface::close(void)
{
    _skeleton.clear();
    return;
}

/*public virtual */void ASDPluginInterface::execute(std::string const& session, insensitive::map<std::string, std::string> const& query, std::string* category, std::string* message)
{
    return;
}

/*public virtual */void ASDPluginInterface::process(std::string const& session, std::string const& category, std::string const& message, std::string* response) const
{
    return;
}

/*public virtual */bool ASDPluginInterface::receive(std::string const& packet)
{
    std::string string;
    int i;
    
    string = (boost::format("Packet received %ld bytes\n") % packet.size()).str();
    for (i = 0; i < packet.size(); ++i) {
        string += (boost::format("%02x") % static_cast<int>(static_cast<unsigned char>(packet[i]))).str();
        string += (i % 16 == 15) ? ("\n") : (" ");
    }
    artsatd::getInstance().log(LOG_NOTICE, "%s", string.c_str());
    return true;
}

/*private static */tgs::TGSError ASDPluginInterface::serializeCache(std::string const& file, std::string* result)
{
    std::ifstream stream;
    std::string line;
    std::string string;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    stream.open(file.c_str());
    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            boost::trim(line);
            string += line;
        }
        *result = string;
        stream.close();
    }
    else {
        error = tgs::TGSERROR_FILE_NOTFOUND;
    }
    return error;
}
