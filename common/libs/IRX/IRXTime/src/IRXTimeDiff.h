/*
**      IridiumFrameworks
**
**      Original Copyright (C) 2012 - 2012 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     C++03
**      Website     http://iridium.jp/
**      E-mail      zap00365@nifty.com
**
**      This source code is for Xcode.
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      IRXTimeDiff.h
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

#ifndef __IR_IRXTIMEDIFF_H
#define __IR_IRXTIMEDIFF_H

#include <time.h>
#include <string>

namespace ir {

//! 時刻差を表現するクラス
/*!
    ２つの時刻の差を扱うには IRXTimeDiff クラスを利用します。
    時刻を扱うには IRXTime クラスを利用します。
 */
class IRXTimeDiff {
    private:
                time_t                  _second;
    
    public:
        /*!
            コンストラクタです。
            値は初期化されません。
         */
        explicit                        IRXTimeDiff                     (void);
        /*!
            コンストラクタです。
            引数に指定した値が初期値となります。
            @param [in] param 初期値
         */
                                        IRXTimeDiff                     (IRXTimeDiff const& param);
        /*!
            コンストラクタです。
            引数に指定した日差・時差・分差・秒差をもとにして初期値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] day 日差
            @param [in] hour 時差
            @param [in] minute 分差
            @param [in] second 秒差
         */
        explicit                        IRXTimeDiff                     (long day, int hour, int minute, int second);
        /*!
            コンストラクタです。
            引数に指定した時差・分差・秒差をもとにして初期値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] hour 時差
            @param [in] minute 分差
            @param [in] second 秒差
         */
        explicit                        IRXTimeDiff                     (long hour, int minute, int second);
        /*!
            コンストラクタです。
            引数に指定した分差・秒差をもとにして初期値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] minute 分差
            @param [in] second 秒差
         */
        explicit                        IRXTimeDiff                     (long minute, int second);
        /*!
            コンストラクタです。
            引数に指定した POSIX 時刻としての秒差をもとにして初期値を設定します。
            @param [in] second POSIX 時刻としての秒差
         */
        explicit                        IRXTimeDiff                     (time_t second);
        /*!
            デストラクタです。
         */
                                        ~IRXTimeDiff                    (void);
        /*!
            値を代入します。
            @param [in] param 代入する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator=                       (IRXTimeDiff const& param);
        /*!
            値を加算します。
            @param [in] param 加算する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator+=                      (IRXTimeDiff const& param);
        /*!
            値を減算します。
            @param [in] param 減算する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator-=                      (IRXTimeDiff const& param);
        /*!
            値を乗算します。
            @param [in] param 乗算する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator*=                      (int param);
        /*!
            値を除算します。
            @param [in] param 除算する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator/=                      (int param);
        /*!
            値の剰余を計算します。
            @param [in] param 除算する値
            @return 自分への参照
         */
                IRXTimeDiff&            operator%=                      (int param);
        /*!
            正の値を計算します。
            @return 計算結果
         */
                IRXTimeDiff             operator+                       (void) const;
        /*!
            負の値を計算します。
            @return 計算結果
         */
                IRXTimeDiff             operator-                       (void) const;
        /*!
            引数に指定した時刻差の要素を取得します。
            @param [in] param 取得する要素（大文字と小文字は区別しない）
                @arg "day" : 日差
                @arg "hour" : 時差
                @arg "minute" : 分差
                @arg "second" : 秒差
            @return 現在の値の絶対値
            @retval <0 指定した要素が存在しない
            @retval >=0 取得に成功
         */
                long                    operator[]                      (std::string const& param) const;
        /*!
            値を設定します。
            @param [in] param 設定する値
            @return 自分への参照
         */
                IRXTimeDiff&            set                             (IRXTimeDiff const& param);
        /*!
            引数に指定した日差・時差・分差・秒差をもとにして値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] day 日差
            @param [in] hour 時差
            @param [in] minute 分差
            @param [in] second 秒差
            @return 自分への参照
         */
                IRXTimeDiff&            set                             (long day, int hour, int minute, int second);
        /*!
            値を日差・時差・分差・秒差に分解して取得します。
            各桁を取得する必要がない場合にはそれぞれの引数に NULL を指定することができます。
            @param [out] day 現在の日差
                @arg NULL : 日差を返さない
                @arg その他
            @param [out] hour 現在の時差
                @arg NULL : 時差を返さない
                @arg その他
            @param [out] minute 現在の分差
                @arg NULL : 分差を返さない
                @arg その他
            @param [out] second 現在の秒差
                @arg NULL : 秒差を返さない
                @arg その他
            @return なし
         */
                void                    get                             (long* day, int* hour, int* minute, int* second) const;
        /*!
            引数に指定した時差・分差・秒差をもとにして値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] hour 時差
            @param [in] minute 分差
            @param [in] second 秒差
            @return 自分への参照
         */
                IRXTimeDiff&            set                             (long hour, int minute, int second);
        /*!
            値を時差・分差・秒差に分解して取得します。
            各桁を取得する必要がない場合にはそれぞれの引数に NULL を指定することができます。
            @param [out] hour 現在の時差
                @arg NULL : 時差を返さない
                @arg その他
            @param [out] minute 現在の分差
                @arg NULL : 分差を返さない
                @arg その他
            @param [out] second 現在の秒差
                @arg NULL : 秒差を返さない
                @arg その他
            @return なし
         */
                void                    get                             (long* hour, int* minute, int* second) const;
        /*!
            引数に指定した分差・秒差をもとにして値を設定します。
            各桁で繰り上げや繰り下げが発生したときにも正しく設定されます。
            @param [in] minute 分差
            @param [in] second 秒差
            @return 自分への参照
         */
                IRXTimeDiff&            set                             (long minute, int second);
        /*!
            値を分差・秒差に分解して取得します。
            各桁を取得する必要がない場合にはそれぞれの引数に NULL を指定することができます。
            @param [out] minute 現在の分差
                @arg NULL : 分差を返さない
                @arg その他
            @param [out] second 現在の秒差
                @arg NULL : 秒差を返さない
                @arg その他
            @return なし
         */
                void                    get                             (long* minute, int* second) const;
        /*!
            引数に指定した POSIX 時刻としての秒差をもとにして値を設定します。
            @param [in] second POSIX 時刻としての秒差
            @return 自分への参照
         */
                IRXTimeDiff&            set                             (time_t second);
        /*!
            値を加算します。
            @param [in] param 加算する値
            @return 自分への参照
         */
                IRXTimeDiff&            add                             (IRXTimeDiff const& param);
        /*!
            値を減算します。
            @param [in] param 減算する値
            @return 自分への参照
         */
                IRXTimeDiff&            sub                             (IRXTimeDiff const& param);
        /*!
            値を乗算します。
            @param [in] param 乗算する値
            @return 自分への参照
         */
                IRXTimeDiff&            mul                             (int param);
        /*!
            値を除算します。
            @param [in] param 除算する値
            @return 自分への参照
         */
                IRXTimeDiff&            div                             (int param);
        /*!
            値の剰余を計算します。
            @param [in] param 除算する値
            @return 自分への参照
         */
                IRXTimeDiff&            mod                             (int param);
        /*!
            負の値を計算します。
            @return 自分への参照
         */
                IRXTimeDiff&            neg                             (void);
        /*!
            値を比較します。
            @param [in] param 比較する値
            @return 比較結果
            @retval true 同じ
            @retval false 同じでない
         */
                bool                    equals                          (IRXTimeDiff const& param) const;
        /*!
            値の大小を比較します。
            @param [in] param 比較する値
            @return 比較結果
            @retval 0 同じ (this == param)
            @retval >0 大きい (this > param)
            @retval <0 小さい (this < param)
         */
                int                     compare                         (IRXTimeDiff const& param) const;
        /*!
            POSIX 時刻としての秒差を取得します。
            @return POSIX 時刻としての秒差
         */
                time_t                  asTime_t                        (void) const;
        /*!
            UTC 時刻からのオフセットを取得します。
            @return 取得結果
         */
        static  IRXTimeDiff             localTimeOffset                 (void);
};

/*!
    値を加算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator+                       (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値を減算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator-                       (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値を乗算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator*                       (IRXTimeDiff const& x, int y);
/*!
    値を乗算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator*                       (int x, IRXTimeDiff const& y);
/*!
    値を除算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator/                       (IRXTimeDiff const& x, int y);
/*!
    値の剰余を計算します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 時刻差
 */
extern  IRXTimeDiff                     operator%                       (IRXTimeDiff const& x, int y);
/*!
    値を比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 同じ
    @retval false 同じでない
 */
extern  bool                            operator==                      (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値を逆比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 同じでない
    @retval false 同じ
 */
extern  bool                            operator!=                      (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値の大小（未満）を比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 真 (this < param)
    @retval false 偽 (this >= param)
 */
extern  bool                            operator<                       (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値の大小（以下）を比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 真 (this <= param)
    @retval false 偽 (this > param)
 */
extern  bool                            operator<=                      (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値の大小（より大きい）を比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 真 (this > param)
    @retval false 偽 (this <= param)
 */
extern  bool                            operator>                       (IRXTimeDiff const& x, IRXTimeDiff const& y);
/*!
    値の大小（以上）を比較します。
    @param [in] x 左辺値
    @param [in] y 右辺値
    @return 比較結果
    @retval true 真 (this >= param)
    @retval false 偽 (this < param)
 */
extern  bool                            operator>=                      (IRXTimeDiff const& x, IRXTimeDiff const& y);

/*public */inline IRXTimeDiff::IRXTimeDiff(void)
{
}

/*public */inline IRXTimeDiff::IRXTimeDiff(IRXTimeDiff const& param)
{
    set(param);
}

/*public */inline IRXTimeDiff::IRXTimeDiff(long day, int hour, int minute, int second)
{
    set(day, hour, minute, second);
}

/*public */inline IRXTimeDiff::IRXTimeDiff(long hour, int minute, int second)
{
    set(hour, minute, second);
}

/*public */inline IRXTimeDiff::IRXTimeDiff(long minute, int second)
{
    set(minute, second);
}

/*public */inline IRXTimeDiff::IRXTimeDiff(time_t second)
{
    set(second);
}

/*public */inline IRXTimeDiff::~IRXTimeDiff(void)
{
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator=(IRXTimeDiff const& param)
{
    return set(param);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator+=(IRXTimeDiff const& param)
{
    return add(param);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator-=(IRXTimeDiff const& param)
{
    return sub(param);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator*=(int param)
{
    return mul(param);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator/=(int param)
{
    return div(param);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::operator%=(int param)
{
    return mod(param);
}

/*public */inline IRXTimeDiff IRXTimeDiff::operator+(void) const
{
    return IRXTimeDiff(+_second);
}

/*public */inline IRXTimeDiff IRXTimeDiff::operator-(void) const
{
    return IRXTimeDiff(-_second);
}

/*public */inline IRXTimeDiff& IRXTimeDiff::set(IRXTimeDiff const& param)
{
    _second = param._second;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::set(time_t second)
{
    _second = second;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::add(IRXTimeDiff const& param)
{
    _second += param._second;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::sub(IRXTimeDiff const& param)
{
    _second -= param._second;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::mul(int param)
{
    _second *= param;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::div(int param)
{
    _second /= param;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::mod(int param)
{
    _second %= param;
    return *this;
}

/*public */inline IRXTimeDiff& IRXTimeDiff::neg(void)
{
    _second = -_second;
    return *this;
}

/*public */inline bool IRXTimeDiff::equals(IRXTimeDiff const& param) const
{
    return (_second == param._second);
}

/*public */inline time_t IRXTimeDiff::asTime_t(void) const
{
    return _second;
}

inline IRXTimeDiff operator+(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return IRXTimeDiff(x).operator+=(y);
}

inline IRXTimeDiff operator-(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return IRXTimeDiff(x).operator-=(y);
}

inline IRXTimeDiff operator*(IRXTimeDiff const& x, int y)
{
    return IRXTimeDiff(x).operator*=(y);
}

inline IRXTimeDiff operator*(int x, IRXTimeDiff const& y)
{
    return IRXTimeDiff(y).operator*=(x);
}

inline IRXTimeDiff operator/(IRXTimeDiff const& x, int y)
{
    return IRXTimeDiff(x).operator/=(y);
}

inline IRXTimeDiff operator%(IRXTimeDiff const& x, int y)
{
    return IRXTimeDiff(x).operator%=(y);
}

inline bool operator==(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return x.equals(y);
}

inline bool operator!=(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return !x.equals(y);
}

inline bool operator<(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return (x.compare(y) < 0);
}

inline bool operator<=(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return (x.compare(y) <= 0);
}

inline bool operator>(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return (x.compare(y) > 0);
}

inline bool operator>=(IRXTimeDiff const& x, IRXTimeDiff const& y)
{
    return (x.compare(y) >= 0);
}

}// end of namespace

#endif
