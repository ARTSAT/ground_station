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
**      TGSError.h
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

#ifndef __TGS_ERROR_H
#define __TGS_ERROR_H

#include <string>

namespace tgs {

enum TGSErrorEnum {
    TGSERROR_OK,
    TGSERROR_FAILED,
    TGSERROR_NO_SUPPORT,
    TGSERROR_NO_RESULT,
    TGSERROR_NO_MEMORY,
    TGSERROR_INVALID_PARAM,
    TGSERROR_INVALID_STATE,
    TGSERROR_INVALID_FORMAT,
    TGSERROR_WAIT_RESULT,
    TGSERROR_FILE_NOTFOUND,
    TGSERROR_FILE_NOTSAVED,
    TGSERROR_NETWORK_DOWN,
    TGSERROR_NETWORK_CODE,
    TGSERROR_TIMEOUT,
    TGSERROR_LIMIT
};

class TGSError {
    private:
                int                         _error;
    
    public:
        explicit                            TGSError                        (void);
                                            TGSError                        (TGSError const& param);
                                            TGSError                        (int param);
                                            ~TGSError                       (void);
                TGSError&                   operator=                       (TGSError const& param);
                TGSError&                   operator=                       (int param);
                                            operator int                    (void) const;
                TGSError&                   set                             (TGSError const& param);
                TGSError&                   set                             (int param);
                int                         get                             (void) const;
                bool                        equals                          (TGSError const& param) const;
                bool                        equals                          (int param) const;
                std::string                 print                           (void) const;
};

extern  bool                                operator==                      (TGSError const& x, TGSError const& y);
extern  bool                                operator==                      (TGSError const& x, int y);
extern  bool                                operator==                      (int x, TGSError const& y);
extern  bool                                operator!=                      (TGSError const& x, TGSError const& y);
extern  bool                                operator!=                      (TGSError const& x, int y);
extern  bool                                operator!=                      (int x, TGSError const& y);

/*public */inline TGSError::TGSError(void)
{
}

/*public */inline TGSError::TGSError(TGSError const& param)
{
    set(param);
}

/*public */inline TGSError::TGSError(int param)
{
    set(param);
}

/*public */inline TGSError::~TGSError(void)
{
}

/*public */inline TGSError& TGSError::operator=(TGSError const& param)
{
    return set(param);
}

/*public */inline TGSError& TGSError::operator=(int param)
{
    return set(param);
}

/*public */inline TGSError::operator int(void) const
{
    return get();
}

/*public */inline TGSError& TGSError::set(TGSError const& param)
{
    _error = param._error;
    return *this;
}

/*public */inline TGSError& TGSError::set(int param)
{
    _error = param;
    return *this;
}

/*public */inline int TGSError::get(void) const
{
    return _error;
}

/*public */inline bool TGSError::equals(TGSError const& param) const
{
    return (_error == param._error);
}

/*public */inline bool TGSError::equals(int param) const
{
    return (_error == param);
}

inline bool operator==(TGSError const& x, TGSError const& y)
{
    return x.equals(y);
}

inline bool operator==(TGSError const& x, int y)
{
    return x.equals(y);
}

inline bool operator==(int x, TGSError const& y)
{
    return y.equals(x);
}

inline bool operator!=(TGSError const& x, TGSError const& y)
{
    return !x.equals(y);
}

inline bool operator!=(TGSError const& x, int y)
{
    return !x.equals(y);
}

inline bool operator!=(int x, TGSError const& y)
{
    return !y.equals(x);
}

}// end of namespace

#endif
