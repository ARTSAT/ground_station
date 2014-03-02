/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
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
**      TGSDatabaseInterface.h
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

#ifndef __TGS_DATABASEINTERFACE_H
#define __TGS_DATABASEINTERFACE_H

#include "TGSType.h"
#include "sqlite3.h"

namespace tgs {

class TGSDatabaseInterface {
    public:
        typedef TGSDatabaseInterface        self;
    
    private:
                sqlite3*                    _database;
                sqlite3_stmt*               _statement;
    
    public:
        virtual                             ~TGSDatabaseInterface           (void) = 0;
        virtual bool                        isValid                         (void) const;
        virtual TGSError                    open                            (std::string const& file);
        virtual void                        close                           (void);
                TGSError                    begin                           (void);
                void                        end                             (void);
                TGSError                    vacuum                          (void);
    protected:
        explicit                            TGSDatabaseInterface            (void);
                TGSError                    create                          (std::string const& table, char const* (*format)[2], unsigned int length);
                TGSError                    update                          (std::string const& table, char const* (*format)[2], unsigned int length, int primary, std::string const& condition, ...);
                TGSError                    update                          (std::string const& table, char const* (*format)[2], unsigned int length, int primary, std::string const& condition, va_list ap);
                TGSError                    remove                          (std::string const& table, char const* (*format)[2], std::string const& condition, ...);
                TGSError                    remove                          (std::string const& table, char const* (*format)[2], std::string const& condition, va_list ap);
                TGSError                    countRow                        (std::string const& table, char const* (*format)[2], std::string const& condition, int* result, ...);
                TGSError                    countRow                        (std::string const& table, char const* (*format)[2], std::string const& condition, int* result, va_list ap);
                TGSError                    select                          (std::string const& table, std::string const& query, char const* (*format)[2], std::string const& condition, ...);
                TGSError                    select                          (std::string const& table, std::string const& query, char const* (*format)[2], std::string const& condition, va_list ap);
                TGSError                    execute                         (std::string const& sql);
                TGSError                    prepare                         (std::string const& sql);
                TGSError                    step                            (void);
                TGSError                    countColumn                     (int* result) const;
                TGSError                    getType                         (int column, int* result) const;
                TGSError                    getSize                         (int column, int* result) const;
                TGSError                    readInteger                     (int column, int* result) const;
                TGSError                    readReal                        (int column, double* result) const;
                TGSError                    readText                        (int column, std::string* result) const;
                TGSError                    checkFlow                       (void) const;
                TGSError                    checkStep                       (void) const;
                TGSError                    checkColumn                     (void* result) const;
        static  TGSError                    print                           (std::string const& format, std::string* result, ...);
        static  TGSError                    print                           (std::string const& format, std::string* result, va_list ap);
        static  TGSError                    buildQuery                      (std::string const& query, char const* (*format)[2], unsigned int length, std::string* result);
        static  TGSError                    buildOrder                      (std::string const& condition, char const* (*format)[2], bool order, int limit, int offset, std::string* result);
    private:
        static  TGSError                    bind                            (char const* (*format)[2], std::string* result, ...);
        static  TGSError                    bind                            (char const* (*format)[2], std::string* result, va_list ap);
    private:
                                            TGSDatabaseInterface            (TGSDatabaseInterface const&);
                TGSDatabaseInterface&       operator=                       (TGSDatabaseInterface const&);
};

}// end of namespace

#endif
