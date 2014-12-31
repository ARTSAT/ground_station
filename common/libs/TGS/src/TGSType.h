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
**      TGSType.h
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

#ifndef __TGS_TYPE_H
#define __TGS_TYPE_H

#include <typeinfo>
#include <string>

namespace tgs {

#define TGS_VERSION_MAJOR       (5)
#define TGS_VERSION_MINOR       (0)
#define TGS_VERSION_REVISION    (0)
#define ID_BASE_TLE             (0)
#define ID_BASE_SCD             (100000)

#define lengthof(param)         (sizeof(param) / sizeof(param[0]))
#define asciiesof(param)        (lengthof(param) - 1)

struct TLERec {
    char                        name[25];
    char                        one[70];
    char                        two[70];
};
struct SCDRec {
    char                        name[65];
    std::string                 info;
    std::string                 param;
};
class OrbitData {
    public:
        enum TypeEnum {
            TYPE_NONE,
            TYPE_TLE,
            TYPE_SCD
        };
    
    private:
                TypeEnum        _type;
                union {
                    TLERec      _tle;
                    SCDRec      _scd;
                };
    
    public:
        explicit                OrbitData               (void);
                                OrbitData               (OrbitData const& param);
                                OrbitData               (TLERec const& param);
                                OrbitData               (SCDRec const& param);
                                ~OrbitData              (void);
                OrbitData&      operator=               (OrbitData const& param);
                OrbitData&      operator=               (TLERec const& param);
                OrbitData&      operator=               (SCDRec const& param);
                                operator TLERec&        (void);
                                operator TLERec const&  (void) const;
                                operator SCDRec&        (void);
                                operator SCDRec const&  (void) const;
                TypeEnum        getType                 (void) const;
};

/*public */inline OrbitData::OrbitData(void) : _type(TYPE_NONE)
{
}

/*public */inline OrbitData::OrbitData(OrbitData const& param) : _type(param._type)
{
    switch (_type) {
        case TYPE_TLE:
            new(&_tle) TLERec(param._tle);
            break;
        case TYPE_SCD:
            new(&_scd) SCDRec(param._scd);
            break;
        default:
            // nop
            break;
    }
}

/*public */inline OrbitData::OrbitData(TLERec const& param) : _type(TYPE_TLE)
{
    new(&_tle) TLERec(param);
}

/*public */inline OrbitData::OrbitData(SCDRec const& param) : _type(TYPE_SCD)
{
    new(&_scd) SCDRec(param);
}

/*public */inline OrbitData::~OrbitData(void)
{
    switch (_type) {
        case TYPE_TLE:
            _tle.~TLERec();
            break;
        case TYPE_SCD:
            _scd.~SCDRec();
            break;
        default:
            // nop
            break;
    }
    _type = TYPE_NONE;
}

/*public */inline OrbitData& OrbitData::operator=(OrbitData const& param)
{
    if (_type == param._type) {
        switch (_type) {
            case TYPE_TLE:
                _tle = param._tle;
                break;
            case TYPE_SCD:
                _scd = param._scd;
                break;
            default:
                // nop
                break;
        }
    }
    else {
        this->~OrbitData();
        new(this) OrbitData(param);
    }
    return *this;
}

/*public */inline OrbitData& OrbitData::operator=(TLERec const& param)
{
    if (_type == TYPE_TLE) {
        _tle = param;
    }
    else {
        this->~OrbitData();
        new(this) OrbitData(param);
    }
    return *this;
}

/*public */inline OrbitData& OrbitData::operator=(SCDRec const& param)
{
    if (_type == TYPE_SCD) {
        _scd = param;
    }
    else {
        this->~OrbitData();
        new(this) OrbitData(param);
    }
    return *this;
}

/*public */inline OrbitData::operator TLERec&(void)
{
    if (_type == TYPE_TLE) {
        return _tle;
    }
    else {
        throw std::bad_cast();
    }
}

/*public */inline OrbitData::operator TLERec const&(void) const
{
    if (_type == TYPE_TLE) {
        return _tle;
    }
    else {
        throw std::bad_cast();
    }
}

/*public */inline OrbitData::operator SCDRec&(void)
{
    if (_type == TYPE_SCD) {
        return _scd;
    }
    else {
        throw std::bad_cast();
    }
}

/*public */inline OrbitData::operator SCDRec const&(void) const
{
    if (_type == TYPE_SCD) {
        return _scd;
    }
    else {
        throw std::bad_cast();
    }
}

/*public */inline OrbitData::TypeEnum OrbitData::getType(void) const
{
    return _type;
}

template <typename T>
class va_ref {
    private:
                T&              _ref;
    
    public:
                                va_ref                  (T& param);
                                ~va_ref                 (void);
                                operator T&             (void);
                                operator T const&       (void) const;
    private:
                va_ref&         operator=               (va_ref const&);
};

template <typename T>
/*public */inline va_ref<T>::va_ref(T& param) : _ref(param)
{
}

template <typename T>
/*public */inline va_ref<T>::~va_ref(void)
{
}

template <typename T>
/*public */inline va_ref<T>::operator T&(void)
{
    return _ref;
}

template <typename T>
/*public */inline va_ref<T>::operator T const&(void) const
{
    return _ref;
}

}// end of namespace

#include "TGSError.h"

namespace tgs {

extern  TGSError                convertTLE              (std::string const& name, std::string const& one, std::string const& two, OrbitData* orbit);
extern  TGSError                convertTLE              (OrbitData const& orbit, std::string* name, std::string* one, std::string* two);
extern  TGSError                convertSCD              (std::string const& name, std::string const& info, std::string const& param, OrbitData* orbit);
extern  TGSError                convertSCD              (OrbitData const& orbit, std::string* name, std::string* info, std::string* param);

}// end of namespace

#endif
