/*
**      Copyright (c) 2004- openFrameworks Community
**      Portions Copyright (C) 2013 - 2015 HORIGUCHI Junshi.
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

#pragma once

// Cross-platform deprecation warning
#ifdef __GNUC__
// clang also has this defined. deprecated(message) is only for gcc>=4.5
#if (__GNUC__ >= 4) && (__GNUC_MINOR__ >= 5)
#define OF_DEPRECATED_MSG(message, func) func __attribute__ ((deprecated(message)))
#else
#define OF_DEPRECATED_MSG(message, func) func __attribute__ ((deprecated))
#endif
#define OF_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OF_DEPRECATED_MSG(message, func) __declspec(deprecated(message)) func
#define OF_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define OF_DEPRECATED_MSG(message, func) func
#define OF_DEPRECATED(func) func
#endif

//-------------------------------
//  find the system type --------
//-------------------------------

// 		helpful:
// 		http://www.ogre3d.org/docs/api/html/OgrePlatform_8h-source.html

#if defined( __WIN32__ ) || defined( _WIN32 )
#define TARGET_WIN32
#elif defined( __APPLE_CC__)
#include <TargetConditionals.h>

#if (TARGET_OS_IPHONE_SIMULATOR) || (TARGET_OS_IPHONE) || (TARGET_IPHONE)
#define TARGET_OF_IPHONE
#define TARGET_OPENGLES
#else
#define TARGET_OSX
#endif
#elif defined (ANDROID)
#define TARGET_ANDROID
#define TARGET_OPENGLES
#else
#define TARGET_LINUX
#endif
//-------------------------------


// then the the platform specific includes:
#ifdef TARGET_WIN32
//this is for TryEnterCriticalSection
//http://www.zeroc.com/forums/help-center/351-ice-1-2-tryentercriticalsection-problem.html
#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x400
#endif
#define WIN32_LEAN_AND_MEAN

#if (_MSC_VER)
#define NOMINMAX
//http://stackoverflow.com/questions/1904635/warning-c4003-and-errors-c2589-and-c2059-on-x-stdnumeric-limitsintmax
#endif

#include <windows.h>
#define GLEW_STATIC
#include "GL\glew.h"
#include "GL\wglew.h"
#include "glu.h"
#define __WINDOWS_DS__
#define __WINDOWS_MM__
#if (_MSC_VER)       // microsoft visual studio
#include <stdint.h>
#pragma warning(disable : 4068)     // unknown pragmas
#pragma warning(disable : 4101)     // unreferenced local variable
#pragma	warning(disable : 4312)		// type cast conversion (in qt vp)
#pragma warning(disable : 4311)		// type cast pointer truncation (qt vp)
#pragma warning(disable : 4018)		// signed/unsigned mismatch (since vector.size() is a size_t)
#pragma warning(disable : 4267)		// conversion from size_t to Size warning... possible loss of data
#pragma warning(disable : 4800)		// 'Boolean' : forcing value to bool 'true' or 'false'
#pragma warning(disable : 4099)		// for debug, PDB 'vc80.pdb' was not found with...
#endif

#define TARGET_LITTLE_ENDIAN			// intel cpu

// note: this is experimental!
// uncomment to turn this on (only for windows machines)
// if you want to try setting the timer to be high resolution
// this could make camera grabbing and other low level
// operations quicker, but you must quit the app normally,
// ie, using "esc", rather than killing the process or closing
// the console window in order to set the timer resolution back
// to normal (since the high res timer might give the OS
// problems)
// info: http://www.geisswerks.com/ryan/FAQS/timing.html

#endif

#ifdef TARGET_OSX
#ifndef __MACOSX_CORE__
#define __MACOSX_CORE__
#endif
#include <unistd.h>
#include <ApplicationServices/ApplicationServices.h>

#if defined(__LITTLE_ENDIAN__)
#define TARGET_LITTLE_ENDIAN		// intel cpu
#endif
#endif

#ifdef TARGET_LINUX
#define GL_GLEXT_PROTOTYPES
#include <unistd.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

// for some reason, this isn't defined at compile time,
// so this hack let's us work
// for 99% of the linux folks that are on intel
// everyone one else will have RGB / BGR issues.
//#if defined(__LITTLE_ENDIAN__)
#define TARGET_LITTLE_ENDIAN		// intel cpu
//#endif

// some things for serial compilation:
#define B14400	14400
#define B28800	28800


#endif


#ifdef TARGET_OF_IPHONE
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#define TARGET_LITTLE_ENDIAN		// arm cpu
#endif

#ifdef TARGET_ANDROID
#include <typeinfo>
#include <unistd.h>
#include <GLES/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES/glext.h>

#define TARGET_LITTLE_ENDIAN
#endif

#ifndef __MWERKS__
#include <cstdlib>
#define OF_EXIT_APP(val)		std::exit(val);
#else
#define OF_EXIT_APP(val)		std::exit(val);
#endif

// serial error codes
#define OF_SERIAL_NO_DATA 	-2
#define OF_SERIAL_ERROR		-1

// core: ---------------------------
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
#include <fstream>
#include <algorithm>
#include <cfloat>


#if defined( TARGET_OSX ) || defined( TARGET_OF_IPHONE ) || defined( TARGET_LINUX ) || defined (TARGET_ANDROID)
	#include <termios.h>
#else
	#include <winbase.h>
	#include <tchar.h>
	#include <iostream>
	#include <string.h>
	#include <setupapi.h>
	#include <regstr.h>
	#define MAX_SERIAL_PORTS 256
	 #include <winioctl.h>
	#ifdef __MINGW32__
			#define INITGUID
			#include <initguid.h> // needed for dev-c++ & DEFINE_GUID
    #endif
#endif

namespace tgs {

class TGSSerialDeviceInfo{
	friend class TGSSerial;
public:
    TGSSerialDeviceInfo(std::string devicePathIn, std::string deviceNameIn, int deviceIDIn){
        devicePath			= devicePathIn;
        deviceName			= deviceNameIn;
        deviceID			= deviceIDIn;
    }
    
    std::string getDevicePath(){
        return devicePath;
    }
    
    std::string getDeviceName(){
        return deviceName;
    }
    
    int getDeviceID(){
        return deviceID;
    }
    
protected:
    std::string devicePath;			//eg: /dev/tty.cu/usbdevice-a440
    std::string deviceName;			//eg: usbdevice-a440 / COM4
    int deviceID;				//eg: 0,1,2,3 etc
};

class TGSSerial {
	public:
			TGSSerial();
			virtual ~TGSSerial();
			std::vector <TGSSerialDeviceInfo> getDeviceList();
			void 			close();
			bool			setup(std::string portName, int baudrate, bool flow);
			bool			writeByte(unsigned char singleByte);
			int             readByte();  // returns -1 on no read or error...
			void			flush(bool flushIn = true, bool flushOut = true);
			int				available();
            void            drain();
    
	protected:
			void			buildDeviceList();
			std::vector <TGSSerialDeviceInfo> devices;
			bool 	bInited;

			#ifdef TARGET_WIN32
				char 		** portNamesShort;//[MAX_SERIAL_PORTS];
				char 		** portNamesFriendly; ///[MAX_SERIAL_PORTS];
				HANDLE  	hComm;		// the handle to the serial port pc
				int	 		nPorts;
				bool 		bPortsEnumerated;
				void 		enumerateWin32Ports();
				COMMTIMEOUTS 	oldTimeout;	// we alter this, so keep a record
			#else
				int 		fd;			// the handle to the serial port mac
				struct 	termios oldoptions;
			#endif
};

}// end of namespace
