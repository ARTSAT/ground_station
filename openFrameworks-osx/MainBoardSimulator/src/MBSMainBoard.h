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
**      MBSMainBoard.h
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

#ifndef __MBS_MAINBOARD_H
#define __MBS_MAINBOARD_H

#include "TGSDeviceInterface.h"

using namespace tgs;

class MBSMainBoard : public TGSDeviceInterface {
    public:
        typedef MBSMainBoard                self;
        typedef TGSDeviceInterface          super;
    
    public:
        class Notifier {
            protected:
                virtual void                onNotifyNSD                     (void);
                virtual void                onNotifyDON                     (void);
                virtual void                onNotifyDOF                     (void);
            friend      class               MBSMainBoard;
        };
    
    private:
                std::vector<std::string>    _queue;
                Notifier*                   _notifier;
    
    public:
        explicit                            MBSMainBoard                    (void);
        virtual                             ~MBSMainBoard                   (void);
                void                        setNotifier                     (Notifier* param);
                Notifier*                   getNotifier                     (void) const;
        virtual TGSError                    open                            (std::string const& port, int baud, bool verbose = true);
        virtual void                        close                           (void);
        virtual TGSError                    update                          (void);
                TGSError                    pushTelemetry                   (unsigned char const* data, int length);
                TGSError                    shutdown                        (void);
    private:
                TGSError                    write                           (std::string const& param);
                TGSError                    read                            (std::string* result);
                void                        poll                            (void);
    private:
                                            MBSMainBoard                    (MBSMainBoard const&);
                MBSMainBoard&               operator=                       (MBSMainBoard const&);
};

/*public */inline void MBSMainBoard::setNotifier(Notifier* param)
{
    _notifier = param;
    return;
}

/*public */inline MBSMainBoard::Notifier* MBSMainBoard::getNotifier(void) const
{
    return _notifier;
}

#endif
