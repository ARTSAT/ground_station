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
**      TGSRotatorGS232B.cpp
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

#include "TGSRotatorGS232B.h"

namespace tgs {

/*public */TGSRotatorGS232B::TGSRotatorGS232B(void)
{
}

/*public virtual */TGSRotatorGS232B::~TGSRotatorGS232B(void)
{
    close();
}

/*public virtual */TGSError TGSRotatorGS232B::getAngle(int* azimuth, int* elevation)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getAngle(azimuth, elevation)) == TGSERROR_NO_SUPPORT) {
        flushRead();
        if ((error = write("C2")) == TGSERROR_OK) {
            if ((error = read(&response)) == TGSERROR_OK) {
                if (sscanf(response.c_str(), "AZ=%03d  EL=%03d", azimuth, elevation) < 2) {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::getAngleAzimuth(int* result)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getAngleAzimuth(result)) == TGSERROR_NO_SUPPORT) {
        flushRead();
        if ((error = write("C")) == TGSERROR_OK) {
            if ((error = read(&response)) == TGSERROR_OK) {
                if (sscanf(response.c_str(), "AZ=%03d", result) < 1) {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::getAngleElevation(int* result)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getAngleElevation(result)) == TGSERROR_NO_SUPPORT) {
        flushRead();
        if ((error = write("B")) == TGSERROR_OK) {
            if ((error = read(&response)) == TGSERROR_OK) {
                if (sscanf(response.c_str(), "EL=%03d", result) < 1) {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::open(std::string const& port, int baud, bool verbose)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if (1200 <= baud && baud <= 9600) {
        if ((error = super::open(port, baud, false, verbose)) == TGSERROR_OK) {
            if ((error = write("")) == TGSERROR_OK) {
                usleep(1000000);
                flushRead();
                if ((error = write("")) == TGSERROR_OK) {
                    if ((error = read(&response)) == TGSERROR_OK) {
                        if (response == "?>") {
                            if (verbose) {
                                std::cout << "TGSRotatorGS232B::open [port : " << port << ", baud : " << baud << "]" << std::endl;
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

/*public virtual */void TGSRotatorGS232B::close(void)
{
    stopManually();
    super::close();
    return;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateTo(int azimuth, int elevation)
{
    char temp[32];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateTo(azimuth, elevation)) == TGSERROR_NO_SUPPORT) {
        if (snprintf(temp, sizeof(temp), "W%03d %03d", azimuth, elevation) >= 0) {
            error = write(temp);
        }
        else {
            error = TGSERROR_FAILED;
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateAzimuthTo(int param)
{
    char temp[32];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateAzimuthTo(param)) == TGSERROR_NO_SUPPORT) {
        if (snprintf(temp, sizeof(temp), "M%03d", param) >= 0) {
            error = write(temp);
        }
        else {
            error = TGSERROR_FAILED;
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateElevationTo(int param)
{
    std::string response;
    int azimuth;
    char temp[32];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateElevationTo(param)) == TGSERROR_NO_SUPPORT) {
        flushRead();
        if ((error = write("C")) == TGSERROR_OK) {
            if ((error = read(&response)) == TGSERROR_OK) {
                if (sscanf(response.c_str(), "AZ=%03d", &azimuth) >= 1) {
                    if (snprintf(temp, sizeof(temp), "W%03d %03d", azimuth, param) >= 0) {
                        error = write(temp);
                    }
                    else {
                        error = TGSERROR_FAILED;
                    }
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateManuallyLeft(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateManuallyLeft()) == TGSERROR_NO_SUPPORT) {
        error = write("L");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateManuallyRight(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateManuallyRight()) == TGSERROR_NO_SUPPORT) {
        error = write("R");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateManuallyUp(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateManuallyUp()) == TGSERROR_NO_SUPPORT) {
        error = write("U");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::rotateManuallyDown(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::rotateManuallyDown()) == TGSERROR_NO_SUPPORT) {
        error = write("D");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::stopManually(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::stopManually()) == TGSERROR_NO_SUPPORT) {
        error = write("S");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::stopManuallyAzimuth(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::stopManuallyAzimuth()) == TGSERROR_NO_SUPPORT) {
        error = write("A");
    }
    return error;
}

/*public virtual */TGSError TGSRotatorGS232B::stopManuallyElevation(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::stopManuallyElevation()) == TGSERROR_NO_SUPPORT) {
        error = write("E");
    }
    return error;
}

/*private */TGSError TGSRotatorGS232B::write(std::string const& param)
{
    return super::write("", param, "\x0D", false);
}

/*private */TGSError TGSRotatorGS232B::read(std::string* result)
{
    return super::read("", result, "\x0D\x0A", false);
}

}// end of namespace
