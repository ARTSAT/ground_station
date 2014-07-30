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
**      ASDNetworkServer.h
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

#ifndef __ASD_NETWORKSERVER_H
#define __ASD_NETWORKSERVER_H

#include "TGSType.h"
#include "IRXTime.h"

namespace insensitive {

template <class T>
struct less : public std::less<T> {
};
template <>
struct less<std::string> : public std::binary_function<std::string, std::string, bool> {
    bool operator()(std::string const& lhs, std::string const& rhs) const
    {
        return boost::ilexicographical_compare(lhs, rhs);
    }
};
template <>
struct less<std::wstring> : public std::binary_function<std::wstring, std::wstring, bool> {
    bool operator()(std::wstring const& lhs, std::wstring const& rhs) const
    {
        return boost::ilexicographical_compare(lhs, rhs);
    }
};
template <class K, class V, class C = less<K>, class A = std::allocator<std::pair<K const, V> > >
struct map : public std::map<K, V, C, A> {
};

}
namespace boost { namespace network { namespace http {

template <class T>
class server;

}}}

class ASDNetworkServer {
    private:
        class Responder;
        typedef boost::network::http::server<Responder>
                                                Server;
    public:
        class Notifier {
            public:
                struct RequestRec {
                    std::string                 host;
                    std::string                 path;
                    insensitive::map<std::string, std::string>
                                                query;
                    insensitive::map<std::string, std::string>
                                                cookie;
                    std::string                 content;
                };
                struct ResponseRec {
                    int                         status;
                    insensitive::map<std::string, std::string>
                                                header;
                    std::string                 content;
                };
            protected:
                typedef Server                  Server;
            
            public:
                static  void                    setCookie                   (std::string const& name, std::string const& value, ResponseRec* response);
                static  void                    setCookie                   (std::string const& name, std::string const& value, std::string const& domain, std::string const& path, bool secure, ResponseRec* response);
                static  void                    setCookie                   (std::string const& name, std::string const& value, unsigned int age, ResponseRec* response);
                static  void                    setCookie                   (std::string const& name, std::string const& value, unsigned int age, std::string const& domain, std::string const& path, bool secure, ResponseRec* response);
                static  void                    setCookie                   (std::string const& name, std::string const& value, ir::IRXTime const& expire, ResponseRec* response);
                static  void                    setCookie                   (std::string const& name, std::string const& value, ir::IRXTime const& expire, std::string const& domain, std::string const& path, bool secure, ResponseRec* response);
                static  void                    replyStatus                 (Server::response::status_type code, ResponseRec* response);
            protected:
                virtual tgs::TGSError           onRequest                   (RequestRec const& request, ResponseRec* response);
            private:
                static  void                    setCookie                   (std::string const& name, std::string const& value, unsigned int const* age, ir::IRXTime const* expire, std::string const& domain, std::string const& path, bool secure, ResponseRec* response);
            friend      class                   ASDNetworkServer;
        };
    
    private:
                boost::scoped_ptr<Responder>    _responder;
                boost::scoped_ptr<Server>       _server;
                boost::scoped_ptr<boost::thread_group>
                                                _thread;
                Notifier*                       _notifier;
    
    public:
        explicit                                ASDNetworkServer            (void);
                                                ~ASDNetworkServer           (void);
                void                            setNotifier                 (Notifier* param);
                Notifier*                       getNotifier                 (void) const;
                tgs::TGSError                   open                        (std::string const& port, int thread);
                void                            close                       (void);
    private:
                                                ASDNetworkServer            (ASDNetworkServer const&);
                ASDNetworkServer&               operator=                   (ASDNetworkServer const&);
};

/*public */inline void ASDNetworkServer::setNotifier(Notifier* param)
{
    _notifier = param;
    return;
}

/*public */inline ASDNetworkServer::Notifier* ASDNetworkServer::getNotifier(void) const
{
    return _notifier;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, ResponseRec* response)
{
    setCookie(name, value, NULL, NULL, "", "", false, response);
    return;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, std::string const& domain, std::string const& path, bool secure, ResponseRec* response)
{
    setCookie(name, value, NULL, NULL, domain, path, secure, response);
    return;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, unsigned int age, ResponseRec* response)
{
    setCookie(name, value, &age, NULL, "", "", false, response);
    return;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, unsigned int age, std::string const& domain, std::string const& path, bool secure, ResponseRec* response)
{
    setCookie(name, value, &age, NULL, domain, path, secure, response);
    return;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, ir::IRXTime const& expire, ResponseRec* response)
{
    setCookie(name, value, NULL, &expire, "", "", false, response);
    return;
}

/*public static */inline void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, ir::IRXTime const& expire, std::string const& domain, std::string const& path, bool secure, ResponseRec* response)
{
    setCookie(name, value, NULL, &expire, domain, path, secure, response);
    return;
}

#endif
