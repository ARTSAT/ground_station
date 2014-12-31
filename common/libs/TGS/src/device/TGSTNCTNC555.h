/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2015 HORIGUCHI Junshi.
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
**      TGSTNCTNC555.h
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

#ifndef __TGS_TNCTNC555_H
#define __TGS_TNCTNC555_H

#include "TGSType.h"
#include "TGSTNCInterface.h"

namespace tgs {

class TGSTNCTNC555 : public TGSTNCInterface {
    public:
        typedef TGSTNCTNC555                self;
        typedef TGSTNCInterface             super;
    
    private:
                std::vector<std::string>    _queue;
    
    public:
        explicit                            TGSTNCTNC555                    (void);
        virtual                             ~TGSTNCTNC555                   (void);
        virtual TGSError                    setPacketMode                   (std::string const& local, std::string const& remote);
        virtual TGSError                    open                            (std::string const& port, int baud, bool verbose = true);
        virtual void                        close                           (void);
        virtual TGSError                    update                          (void);
        virtual TGSError                    selectModeCommand               (void);
        virtual TGSError                    selectModeConverse              (void);
        virtual TGSError                    sendPacket                      (std::string const& packet);
    private:
                TGSError                    set                             (std::string const& command, std::string const& value);
                TGSError                    write                           (std::string const& param);
                TGSError                    read                            (std::string* result);
                void                        poll                            (void);
    private:
                                            TGSTNCTNC555                    (TGSTNCTNC555 const&);
                TGSTNCTNC555&               operator=                       (TGSTNCTNC555 const&);
};

}// end of namespace

#endif
