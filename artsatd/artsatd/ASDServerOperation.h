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
**      ASDServerOperation.h
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

#ifndef __ASD_SERVEROPERATION_H
#define __ASD_SERVEROPERATION_H

#include "ASDNetworkServer.h"
#include "ASDServerRPC.h"

class ASDServerOperation : public ASDNetworkServer::Notifier {
    private:
        struct CacheRec {
            std::string                         mime;
            std::string                         cache;
            std::string                         content;
            void          (ASDServerOperation::*function)                   (RequestRec const& request, ResponseRec* response);
        };
    
    private:
                insensitive::map<std::string, CacheRec>
                                                _cache;
                std::map<std::string, ASDServerRPC::Method>
                                                _method;
                std::string                     _database;
    
    public:
        explicit                                ASDServerOperation          (void);
        virtual                                 ~ASDServerOperation         (void);
                tgs::TGSError                   open                        (std::string const& skeleton, std::string const& database);
                void                            close                       (void);
                void                            replyRoot                   (RequestRec const& request, ResponseRec* response);
                void                            replyHardware               (RequestRec const& request, ResponseRec* response);
                void                            replyOrbital                (RequestRec const& request, ResponseRec* response);
                void                            replyJSONRPC                (RequestRec const& request, ResponseRec* response);
    private:
        virtual tgs::TGSError                   onRequest                   (RequestRec const& request, ResponseRec* response);
        static  tgs::TGSError                   serializeCache              (std::string const& file, std::string* result);
        static  void                            bindError                   (std::string const& name, tgs::TGSError error, std::string* category, std::string* message);
        static  std::string                     colorizeSpan                (std::string const& color, std::string const& string);
        static  std::string                     stringizeLatitude           (double param);
        static  std::string                     stringizeLongitude          (double param);
        static  std::string                     stringizeAltitude           (double param);
        static  std::string                     stringizeAzimuth            (int param);
        static  std::string                     stringizeAzimuth            (double param);
        static  std::string                     stringizeElevation          (int param);
        static  std::string                     stringizeElevation          (double param);
        static  std::string                     stringizeNORAD              (int param);
        static  std::string                     stringizeCallsign           (std::string const& param);
        static  std::string                     stringizeMode               (std::string const& param);
        static  std::string                     stringizeFrequency          (int param);
        static  std::string                     stringizeFrequency          (double param);
        static  std::string                     stringizeDrift              (int param);
        static  std::string                     stringizeDopplerShift       (double param);
        static  std::string                     stringizeTLE                (tgs::TLERec const& param);
        static  std::string                     stringizeTime               (ir::IRXTime const& param);
        static  std::string                     stringizeTimeDiff           (ir::IRXTimeDiff const& param);
        static  std::string                     stringizeSession            (std::string const& param);
        static  std::string                     stringizeOnline             (int param);
    private:
                                                ASDServerOperation          (ASDServerOperation const&);
                ASDServerOperation&             operator=                   (ASDServerOperation const&);
};

#endif
