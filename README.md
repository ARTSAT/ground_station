ground_station
===

ARTSAT ground station software for amateur satellites.
Programmed with C++ language and posix APIs.
This is a daemon software which runs as background task.
User interface is provided as a web interface.
You can access and control this software with Safari or with another web browser.

Development environment:
o MacOS X 10.9.3
o Xcode 5.1.1
o boost 1.55.0
o cpp-netlib 0.11.0

First you have to compile and to install boost and cpp-netlib libraries.
Here are some patch files to do that:
http://github.com/toolbits/boost_1_55_0_xcode510_universal
http://github.com/toolbits/cpp-netlib-0.11.0-final_xcode510_universal

Then open artsatd/artsatd.xcodeproj and do compile it.
Have fun!
