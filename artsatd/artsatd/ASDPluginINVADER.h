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
**      ASDPluginINVADER.h
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

#ifndef __ASD_PLUGININVADER_H
#define __ASD_PLUGININVADER_H

#include "ASDPluginInterface.h"

class ASDPluginINVADER : public ASDPluginInterface {
    public:
        typedef ASDPluginINVADER                self;
        typedef ASDPluginInterface              super;
    
    private:
        enum TextEnum {
            TEXT_X,
            TEXT_Y,
            TEXT_Z,
            TEXT_DEBUG,
            TEXT_LIMIT
        };
    
    private:
        struct CommandRec {
            std::string                         address;
            std::string                         offset;
            std::string                         count;
            std::string                         ptd_address;
            std::string                         ptd_offset;
            std::string                         ptd_count;
            std::string                         mode;
            std::string                         text;
            std::string                         vm;
            std::string                         param;
            std::string                         note;
            std::string                         morse;
            std::string                         digitalker;
            std::string                         camera;
            std::string                         line;
            std::string                         obc;
            std::string                         ohour;
            std::string                         ominute;
            std::string                         osecond;
            std::string                         dhour;
            std::string                         dminute;
            std::string                         dsecond;
        };
    
    private:
                CommandRec                      _command;
        mutable boost::mutex                    _mutex;
    
    public:
        explicit                                ASDPluginINVADER            (void);
        virtual                                 ~ASDPluginINVADER           (void);
        virtual void                            execute                     (std::string const& session, insensitive::map<std::string, std::string> const& query, std::string* category, std::string* message);
        virtual void                            process                     (std::string const& session, std::string const& category, std::string const& message, std::string* response) const;
    private:
                tgs::TGSError                   queueSTA                    (std::string const& session);
                tgs::TGSError                   queueMRR                    (std::string const& session);
                tgs::TGSError                   queuePTI                    (std::string const& session);
                tgs::TGSError                   queuePTR                    (std::string const& session, std::string const& address);
                tgs::TGSError                   queuePTR                    (std::string const& session, int address);
                tgs::TGSError                   queueDDS                    (std::string const& session, bool param);
                tgs::TGSError                   queuePTD                    (std::string const& session, std::string const& address);
                tgs::TGSError                   queuePTD                    (std::string const& session, int address);
                tgs::TGSError                   queueGWABootMode            (std::string const& session, std::string const& mode);
                tgs::TGSError                   queueGWABootMode            (std::string const& session, int mode);
                tgs::TGSError                   queueGRSBootMode            (std::string const& session);
                tgs::TGSError                   queueGWAText                (std::string const& session, std::string const& text, std::string const& data);
                tgs::TGSError                   queueGWAText                (std::string const& session, TextEnum text, std::string const& data);
                tgs::TGSError                   queueGRSText                (std::string const& session, std::string const& text);
                tgs::TGSError                   queueGRSText                (std::string const& session, TextEnum text);
                tgs::TGSError                   queueGWAVM                  (std::string const& session, std::string const& data);
                tgs::TGSError                   queueGRSVM                  (std::string const& session);
                tgs::TGSError                   queueGWANote                (std::string const& session, std::string const& data);
                tgs::TGSError                   queueGRSNote                (std::string const& session);
                tgs::TGSError                   queueGWAMorse               (std::string const& session, std::string const& data);
                tgs::TGSError                   queueGRSMorse               (std::string const& session);
                tgs::TGSError                   queueGWADigitalker          (std::string const& session, std::string const& data);
                tgs::TGSError                   queueGRSDigitalker          (std::string const& session);
                tgs::TGSError                   queueGWACamera              (std::string const& session, std::string const& data);
                tgs::TGSError                   queueGRSCamera              (std::string const& session);
                tgs::TGSError                   queueGWA                    (std::string const& session, int address, std::string const& data, bool size);
                tgs::TGSError                   queueGRS                    (std::string const& session, int address, int size);
                tgs::TGSError                   queueMSR                    (std::string const& session);
                tgs::TGSError                   queueMSD                    (std::string const& session);
                tgs::TGSError                   queueDON                    (std::string const& session);
                tgs::TGSError                   queueDOF                    (std::string const& session);
        static  int                             fromHex                     (char param);
    private:
                                                ASDPluginINVADER            (ASDPluginINVADER const&);
                ASDPluginINVADER&               operator=                   (ASDPluginINVADER const&);
};

#endif
