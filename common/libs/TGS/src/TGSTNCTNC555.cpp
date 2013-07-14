/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
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
**      TGSTNCTNC555.cpp
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

#include "TGSTNCTNC555.h"

namespace tgs {

/*public */TGSTNCTNC555::TGSTNCTNC555(void)
{
}

/*public virtual */TGSTNCTNC555::~TGSTNCTNC555(void)
{
    close();
}

/*public virtual */TGSError TGSTNCTNC555::open(std::string const& port, int baud, bool verbose)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if (300 <= baud && baud <= 19200) {
        if ((error = super::open(port, baud, true, verbose)) == TGSERROR_OK) {
            flushRead();
            if ((error = write("ECHO ON")) == TGSERROR_OK) {
                read(&response);
                flushRead();
                if ((error = write("ECHO ON")) == TGSERROR_OK) {
                    if ((error = read(&response)) == TGSERROR_OK) {
                        if (response == "ECHO ON""\x0D\x0A""ECHO     was ON") {
                            if (verbose) {
                                std::cout << "TGSTNCTNC555::open [port : " << port << ", baud : " << baud << "]" << std::endl;
                            }
                        }
                        else {
                            error = TGSERROR_INVALID_STATE;
                        }
                    }
                }
            }
            if (error != TGSERROR_OK) {
                self::close();
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public virtual */void TGSTNCTNC555::close(void)
{
    super::close();
    return;
}

/*public virtual */TGSError TGSTNCTNC555::setupModePacket(std::string const& local, std::string const& remote)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setupModePacket(local, remote)) == TGSERROR_NO_SUPPORT) {
        if ((error = set("MYCALL", local)) == TGSERROR_OK) {
            if ((error = set("UNPROTO", remote)) == TGSERROR_OK) {
                error = set("FULLDUP", "ON");
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSTNCTNC555::selectModeCommand(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::selectModeCommand()) == TGSERROR_NO_SUPPORT) {
        error = set("\x03", "");
    }
    return error;
}

/*public virtual */TGSError TGSTNCTNC555::selectModeConverse(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::selectModeConverse()) == TGSERROR_NO_SUPPORT) {
        error = set("CONVERSE", "");
    }
    return error;
}

/*private */TGSError TGSTNCTNC555::set(std::string const& command, std::string const& value)
{
    std::string request;
    std::string response;
    TGSError error(TGSERROR_OK);
    
    request += command;
    if (!value.empty()) {
        request += " ";
        request += value;
        flushRead();
    }
    if ((error = write(request)) == TGSERROR_OK) {
        if (!value.empty()) {
            if ((error = read(&response)) == TGSERROR_OK) {
                if (response.find(request) != 0) {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
    }
    return error;
}

/*private */TGSError TGSTNCTNC555::write(std::string const& param)
{
    return super::write("", param, "\x0D", false);
}

/*private */TGSError TGSTNCTNC555::read(std::string* result)
{
    return super::read("", result, "\x0D\x0A""cmd:", false);
}

}// end of namespace
