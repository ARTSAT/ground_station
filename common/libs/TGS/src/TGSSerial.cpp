/*
**      Copyright (c) 2004- openFrameworks Community
**      Portions Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
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

#include "TGSSerial.h"

#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	#include <sys/ioctl.h>
	#include <getopt.h>
	#include <dirent.h>
#endif


#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <algorithm>

namespace tgs {

//---------------------------------------------
#ifdef TARGET_WIN32
//---------------------------------------------

//------------------------------------
   // needed for serial bus enumeration:
   //4d36e978-e325-11ce-bfc1-08002be10318}
   DEFINE_GUID (GUID_SERENUM_BUS_ENUMERATOR, 0x4D36E978, 0xE325,
   0x11CE, 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18);
//------------------------------------

void TGSSerial::enumerateWin32Ports(){

    // thanks joerg for fixes...

	if (bPortsEnumerated == true) return;

	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInterfaceData;
	int i = 0;
	DWORD dataType, actualSize = 0;
	unsigned char dataBuf[MAX_PATH + 1];

	// Reset Port List
	nPorts = 0;
	// Search device set
	hDevInfo = SetupDiGetClassDevs((struct _GUID *)&GUID_SERENUM_BUS_ENUMERATOR,0,0,DIGCF_PRESENT);
	if ( hDevInfo ){
      while (TRUE){
         ZeroMemory(&DeviceInterfaceData, sizeof(DeviceInterfaceData));
         DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
         if (!SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInterfaceData)){
             // SetupDiEnumDeviceInfo failed
             break;
         }

         if (SetupDiGetDeviceRegistryPropertyA(hDevInfo,
             &DeviceInterfaceData,
             SPDRP_FRIENDLYNAME,
             &dataType,
             dataBuf,
             sizeof(dataBuf),
             &actualSize)){

			sprintf(portNamesFriendly[nPorts], "%s", dataBuf);
			portNamesShort[nPorts][0] = 0;

			// turn blahblahblah(COM4) into COM4

            char *   begin    = NULL;
            char *   end    = NULL;
            begin          = strstr((char *)dataBuf, "COM");


            if (begin)
                {
                end          = strstr(begin, ")");
                if (end)
                    {
                      *end = 0;   // get rid of the )...
                      strcpy(portNamesShort[nPorts], begin);
                }
                if (nPorts++ > MAX_SERIAL_PORTS)
                        break;
            }
         }
            i++;
      }
   }
   SetupDiDestroyDeviceInfoList(hDevInfo);

   bPortsEnumerated = false;
}


//---------------------------------------------
#endif
//---------------------------------------------



//----------------------------------------------------------------
TGSSerial::TGSSerial(){

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		nPorts 				= 0;
		bPortsEnumerated 	= false;

		portNamesShort = new char * [MAX_SERIAL_PORTS];
		portNamesFriendly = new char * [MAX_SERIAL_PORTS];
		for (int i = 0; i < MAX_SERIAL_PORTS; i++){
			portNamesShort[i] = new char[10];
			portNamesFriendly[i] = new char[MAX_PATH];
		}
	//---------------------------------------------
	#endif
	//---------------------------------------------
	bInited = false;
}

//----------------------------------------------------------------
TGSSerial::~TGSSerial(){

	close();



	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		nPorts 				= 0;
		bPortsEnumerated 	= false;

		for (int i = 0; i < MAX_SERIAL_PORTS; i++) {
			delete [] portNamesShort[i];
			delete [] portNamesFriendly[i];
		}
		delete [] portNamesShort;
		delete [] portNamesFriendly;

	//---------------------------------------------
	#endif
	//---------------------------------------------

	bInited = false;
}

//----------------------------------------------------------------
static bool isDeviceArduino( TGSSerialDeviceInfo & A ){
	//TODO - this should be ofStringInString
	return ( strstr(A.getDeviceName().c_str(), "usbserial") != NULL ||
		 strstr(A.getDeviceName().c_str(), "usbmodem") != NULL );
}

//----------------------------------------------------------------
void TGSSerial::buildDeviceList(){
	devices.clear();
	
	std::vector <std::string> prefixMatch;

	#ifdef TARGET_OSX
		prefixMatch.push_back("cu.");
		prefixMatch.push_back("tty.");
	#endif
	#ifdef TARGET_LINUX
		prefixMatch.push_back("ttyS");
		prefixMatch.push_back("ttyUSB");
		prefixMatch.push_back("rfc");
	#endif	
	
	
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )

	DIR *dir;
	struct dirent *entry;
	dir = opendir("/dev");
	
	std::string deviceName	= "";
	int deviceCount		= 0;
	
	if (dir == NULL){
		//std::cerr << "TGSSerial: error listing devices in /dev" << std::endl;
	} else {		
		//for each device
		while((entry = readdir(dir)) != NULL){
			deviceName = (char *)entry->d_name;
			
			//we go through the prefixes 
			for(int k = 0; k < (int)prefixMatch.size(); k++){
				//if the device name is longer than the prefix
				if( deviceName.size() > prefixMatch[k].size() ){
					//do they match ?
					if( deviceName.substr(0, prefixMatch[k].size()) == prefixMatch[k].c_str() ){
						devices.push_back(TGSSerialDeviceInfo("/dev/"+deviceName, deviceName, deviceCount));
						deviceCount++;
						break;
					}
				}
			}
		}
		closedir(dir);		
	}
	
	#endif	

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
	enumerateWin32Ports();
	for (int i = 0; i < nPorts; i++){
		//NOTE: we give the short port name for both as that is what the user should pass and the short name is more friendly
		devices.push_back(TGSSerialDeviceInfo(string(portNamesShort[i]), string(portNamesShort[i]), i));
	}
	//---------------------------------------------
	#endif
    //---------------------------------------------
	
	//here we sort the device to have the aruino ones first. 
	partition(devices.begin(), devices.end(), isDeviceArduino);
	//we are reordering the device ids. too!
	for(int k = 0; k < (int)devices.size(); k++){
		devices[k].deviceID = k;
	}
}

//----------------------------------------------------------------
std::vector <TGSSerialDeviceInfo> TGSSerial::getDeviceList(){
	buildDeviceList();
	return devices;
}

//----------------------------------------------------------------
void TGSSerial::close(){

	//---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------
		if (bInited){
			SetCommTimeouts(hComm,&oldTimeout);
			CloseHandle(hComm);
			hComm 		= INVALID_HANDLE_VALUE;
			bInited 	= false;
		}
	//---------------------------------------------
    #else
    //---------------------------------------------
    	if (bInited){
    		tcsetattr(fd,TCSANOW,&oldoptions);
    		::close(fd);
    	}
    	// [CHECK] -- anything else need to be reset?
    //---------------------------------------------
    #endif
    //---------------------------------------------

}

//----------------------------------------------------------------
bool TGSSerial::setup(std::string portName, int baud, bool flow){

	bInited = false;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	//---------------------------------------------
	
		//lets account for the name being passed in instead of the device path
		if( portName.size() > 5 && portName.substr(0, 5) != "/dev/" ){
			portName = "/dev/" + portName;
		}

		fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXLOCK);
		if(fd == -1){
			//std::cerr << "TGSSerial: unable to open port " << portName << std::endl;
			return false;
		}

		struct termios options;
		tcgetattr(fd,&oldoptions);
		options = oldoptions;
		switch(baud){
		   case 300: 	cfsetispeed(&options,B300);
						cfsetospeed(&options,B300);
						break;
		   case 1200: 	cfsetispeed(&options,B1200);
						cfsetospeed(&options,B1200);
						break;
		   case 2400: 	cfsetispeed(&options,B2400);
						cfsetospeed(&options,B2400);
						break;
		   case 4800: 	cfsetispeed(&options,B4800);
						cfsetospeed(&options,B4800);
						break;
		   case 9600: 	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						break;
		   case 14400: 	cfsetispeed(&options,B14400);
						cfsetospeed(&options,B14400);
						break;
		   case 19200: 	cfsetispeed(&options,B19200);
						cfsetospeed(&options,B19200);
						break;
		   case 28800: 	cfsetispeed(&options,B28800);
						cfsetospeed(&options,B28800);
						break;
		   case 38400: 	cfsetispeed(&options,B38400);
						cfsetospeed(&options,B38400);
						break;
		   case 57600:  cfsetispeed(&options,B57600);
						cfsetospeed(&options,B57600);
						break;
		   case 115200: cfsetispeed(&options,B115200);
						cfsetospeed(&options,B115200);
						break;
		   case 230400: cfsetispeed(&options,B230400);
						cfsetospeed(&options,B230400);
						break;

			default:	cfsetispeed(&options,B9600);
						cfsetospeed(&options,B9600);
						//std::cerr << "TGSSerialInit: cannot set " << baud << " baud setting baud to 9600" << std::endl;
						break;
		}

        options.c_cflag &= ~CRTSCTS;
        if (flow) {
            options.c_cflag |= CRTSCTS;
            //options.c_cflag |= CDTR_IFLOW | CDSR_OFLOW | CCAR_OFLOW;
        }
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;
		tcsetattr(fd,TCSANOW,&options);

		bInited = true;
	    return true;
	//---------------------------------------------
    #endif
    //---------------------------------------------


    //---------------------------------------------
	#ifdef TARGET_WIN32
	//---------------------------------------------

	// open the serial port:
	// "COM4", etc...

	hComm=CreateFileA(portName.c_str(),GENERIC_READ|GENERIC_WRITE,0,0,
					OPEN_EXISTING,0,0);

	if(hComm==INVALID_HANDLE_VALUE){
		//cerr << "TGSSerial: unable to open port" << endl;
		return false;
	}


	// now try the settings:
	COMMCONFIG cfg;
	DWORD cfgSize;
	char  buf[80];

	cfgSize=sizeof(cfg);
	GetCommConfig(hComm,&cfg,&cfgSize);
	int bps = baud;
	sprintf(buf,"baud=%d parity=N data=8 stop=1",bps);

	#if (_MSC_VER)       // microsoft visual studio
		// msvc doesn't like BuildCommDCB,
		//so we need to use this version: BuildCommDCBA
		if(!BuildCommDCBA(buf,&cfg.dcb)){
			//cerr << "TGSSerial: unable to build comm dcb; (" << buf << ")" << endl;
		}
	#else
		if(!BuildCommDCB(buf,&cfg.dcb)){
			//cerr << "TGSSerial: Can't build comm dcb; " << buf << endl;
		}
	#endif


	// Set baudrate and bits etc.
	// Note that BuildCommDCB() clears XON/XOFF and hardware control by default

	if(!SetCommState(hComm,&cfg.dcb)){
		//cerr << "TGSSerial: Can't set comm state" << endl;
	}

	// Set communication timeouts (NT)
	COMMTIMEOUTS tOut;
	GetCommTimeouts(hComm,&oldTimeout);
	tOut = oldTimeout;
	// Make timeout so that:
	// - return immediately with buffered characters
	tOut.ReadIntervalTimeout=MAXDWORD;
	tOut.ReadTotalTimeoutMultiplier=0;
	tOut.ReadTotalTimeoutConstant=0;
	SetCommTimeouts(hComm,&tOut);

	bInited = true;
	return true;
	//---------------------------------------------
	#endif
	//---------------------------------------------
}

//----------------------------------------------------------------
bool TGSSerial::writeByte(unsigned char singleByte){

	if (!bInited){
		//std::cerr << "TGSSerial: serial not inited" << std::endl;
		return false;
	}

	unsigned char tmpByte[1];
	tmpByte[0] = singleByte;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
	    ssize_t numWritten = 0;
	    numWritten = write(fd, tmpByte, 1);
		if(numWritten <= 0 ){
			if ( errno == EAGAIN )
				return 0;
            //std::cerr << "TGSSerial: Can't write to com port, errno " << errno << " (" << strerror(errno) << ")" << std::endl;
            return OF_SERIAL_ERROR;
		}
		return (numWritten > 0 ? true : false);
    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD written = 0;
		if(!WriteFile(hComm, tmpByte, 1, &written,0)){
			 //cerr << "TGSSerial: Can't write to com port" << endl;
			 return OF_SERIAL_ERROR;;
		}
		return ((int)written > 0 ? true : false);
	#endif
	//---------------------------------------------

}

//----------------------------------------------------------------
int TGSSerial::readByte(){

	if (!bInited){
		//std::cerr << "TGSSerial: serial not inited" << std::endl;
		return OF_SERIAL_ERROR;
	}

	unsigned char tmpByte[1];
	memset(tmpByte, 0, 1);

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		ssize_t nRead = read(fd, tmpByte, 1);
		if(nRead < 0){
			if ( errno == EAGAIN )
				return OF_SERIAL_NO_DATA;
			//std::cerr << "TGSSerial: trouble reading from port, errno " << errno << " (" << strerror(errno) << ")" << std::endl;
            return OF_SERIAL_ERROR;
		}
		if(nRead == 0)
			return OF_SERIAL_NO_DATA;
    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		DWORD nRead;
		if (!ReadFile(hComm, tmpByte, 1, &nRead, 0)){
			//cerr << "TGSSerial: trouble reading from port" << endl;
			return OF_SERIAL_ERROR;
		}
	#endif
	//---------------------------------------------

	return (int)(tmpByte[0]);
}


//----------------------------------------------------------------
void TGSSerial::flush(bool flushIn, bool flushOut){

	if (!bInited){
		//std::cerr << "TGSSerial: serial not inited" << std::endl;
		return;
	}

	int flushType = 0;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		if( flushIn && flushOut) flushType = TCIOFLUSH;
		else if(flushIn) flushType = TCIFLUSH;
		else if(flushOut) flushType = TCOFLUSH;
		else return;

		tcflush(fd, flushType);
    #endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
		if( flushIn && flushOut) flushType = PURGE_TXCLEAR | PURGE_RXCLEAR;
		else if(flushIn) flushType = PURGE_RXCLEAR;
		else if(flushOut) flushType = PURGE_TXCLEAR;
		else return;

		PurgeComm(hComm, flushType);
	#endif
	//---------------------------------------------

}

void TGSSerial::drain(){
    if (!bInited){
	//std::cerr << "TGSSerial: serial not inited" << std::endl;
	return;
    }

    #if defined( TARGET_OSX ) || defined( TARGET_LINUX )
        tcdrain( fd );
    #endif
}

//-------------------------------------------------------------
int TGSSerial::available(){

	if (!bInited){
		//std::cerr << "TGSSerial: serial not inited" << std::endl;
		return OF_SERIAL_ERROR;
	}

	int numBytes = 0;

	//---------------------------------------------
	#if defined( TARGET_OSX ) || defined( TARGET_LINUX )
		ioctl(fd,FIONREAD,&numBytes);
	#endif
    //---------------------------------------------

    //---------------------------------------------
	#ifdef TARGET_WIN32
	COMSTAT stat;
       	DWORD err;
       	if(hComm!=INVALID_HANDLE_VALUE){
           if(!ClearCommError(hComm, &err, &stat)){
               numBytes = 0;
           } else {
               numBytes = stat.cbInQue;
           }
       	} else {
           numBytes = 0;
       	}
	#endif
    //---------------------------------------------

	return numBytes;
}

}// end of namespace
