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
**      TGSPhysicsDatabase.h
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

#ifndef __TGS_PHYSICSDATABASE_H
#define __TGS_PHYSICSDATABASE_H

#include "TGSType.h"
#include "TGSDatabaseInterface.h"
#include <vector>
#include "IRXTime.h"

namespace tgs {

class TGSPhysicsDatabase : public TGSDatabaseInterface {
    public:
        typedef TGSPhysicsDatabase          self;
        typedef TGSDatabaseInterface        super;
    
    public:
        struct RadioRec {
            std::string                     mode;
            int                             frequency;  // 0 <= N, -1 is invalid
            int                             drift;      // INT_MIN < N <= INT_MAX, INT_MIN is invalid
        };
        struct FieldRec {
            int                             norad;      // 0 <= N, -1 is invalid
            std::string                     name;
            std::string                     callsign;
            RadioRec                        beacon;
            RadioRec                        sender;
            RadioRec                        receiver;
            ir::IRXTime                     time;
            OrbitData                       orbit;
        };
    
    public:
        explicit                            TGSPhysicsDatabase              (void);
        virtual                             ~TGSPhysicsDatabase             (void);
                TGSError                    setName                         (int norad, std::string const& param);
                TGSError                    getName                         (int norad, std::string* result);
                TGSError                    setCallsign                     (int norad, std::string const& param);
                TGSError                    getCallsign                     (int norad, std::string* result);
                TGSError                    setRadioBeacon                  (int norad, RadioRec const& param);
                TGSError                    getRadioBeacon                  (int norad, RadioRec* result);
                TGSError                    setRadioSender                  (int norad, RadioRec const& param);
                TGSError                    getRadioSender                  (int norad, RadioRec* result);
                TGSError                    setRadioReceiver                (int norad, RadioRec const& param);
                TGSError                    getRadioReceiver                (int norad, RadioRec* result);
                TGSError                    setOrbitData                    (OrbitData const& param, ir::IRXTime const& time);
                TGSError                    getOrbitData                    (int norad, OrbitData* result, ir::IRXTime* time = NULL);
                TGSError                    getCount                        (int* result);
                TGSError                    getField                        (int norad, FieldRec* result);
                TGSError                    getField                        (int limit, int offset, std::vector<FieldRec>* result);
                TGSError                    getFieldByName                  (std::string const& name, std::vector<FieldRec>* result);
                TGSError                    getFieldByCallsign              (std::string const& callsign, std::vector<FieldRec>* result);
                TGSError                    getNORADByName                  (std::string const& name, std::vector<int>* result);
                TGSError                    getNORADByCallsign              (std::string const& callsign, std::vector<int>* result);
                bool                        hasUpdate                       (int norad);
        virtual TGSError                    open                            (std::string const& file, int timeout = 10000);
        virtual void                        close                           (void);
    private:
                TGSError                    setText                         (int norad, char const* format[3][2], std::string const& param);
                TGSError                    getText                         (int norad, char const* format[2][2], std::string* result);
                TGSError                    setRadio                        (int norad, char const* format[5][2], RadioRec const& param);
                TGSError                    getRadio                        (int norad, char const* format[4][2], RadioRec* result);
                TGSError                    getField                        (std::string const& key, int index, std::vector<FieldRec>* result);
                TGSError                    getNORAD                        (std::string const& key, int index, std::vector<int>* result);
                TGSError                    readField                       (int column, FieldRec* result) const;
                TGSError                    readInteger                     (int column, bool implicit, int* result) const;
                TGSError                    readText                        (int column, bool implicit, std::string* result) const;
    private:
                                            TGSPhysicsDatabase              (TGSPhysicsDatabase const&);
                TGSPhysicsDatabase&         operator=                       (TGSPhysicsDatabase const&);
};

}// end of namespace

#endif
