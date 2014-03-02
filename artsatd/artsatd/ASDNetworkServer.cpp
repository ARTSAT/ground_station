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
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      ASDNetworkServer.cpp
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

#include "ASDNetworkServer.h"
#include "artsatd.h"

namespace boost { namespace network { namespace uri {
namespace details {
template <
typename Map
>
struct key_value_sequence_mod
: spirit::qi::grammar<uri::const_iterator, Map()>
{
    typedef typename Map::key_type key_type;
    typedef typename Map::mapped_type mapped_type;
    typedef std::pair<key_type, mapped_type> pair_type;
    
    key_value_sequence_mod()
    : key_value_sequence_mod::base_type(query)
    {
        query =  pair >> *((spirit::qi::lit(';') | '&') >> pair);
        pair  =  key >> -('=' >> value);
        key   =  spirit::qi::char_("a-zA-Z_0-9%+") >> *spirit::qi::char_("a-zA-Z_0-9/%+");
        value = *spirit::qi::char_("a-zA-Z_0-9/%+");
    }
    
    spirit::qi::rule<uri::const_iterator, Map()> query;
    spirit::qi::rule<uri::const_iterator, pair_type()> pair;
    spirit::qi::rule<uri::const_iterator, key_type()> key;
    spirit::qi::rule<uri::const_iterator, mapped_type()> value;
};
}

template <
class Map
>
inline
Map &query_map_mod(const uri &uri_, Map &map) {
    const uri::string_type range = uri_.query();
    details::key_value_sequence_mod<Map> parser;
    spirit::qi::parse(boost::begin(range), boost::end(range), parser, map);
    return map;
}
}}}

class ASDNetworkServer::Responder {
    private:
                ASDNetworkServer const* _server;
    
    public:
        explicit                        Responder       (ASDNetworkServer const* param);
                                        ~Responder      (void);
                void                    operator()      (Server::request const& request, Server::response& response);
                void                    log             (Server::string_type const& info);
    private:
                                        Responder       (Responder const&);
                Responder&              operator=       (Responder const&);
};

/*public */ASDNetworkServer::ASDNetworkServer(void)
{
    _notifier = NULL;
}

/*public */ASDNetworkServer::~ASDNetworkServer(void)
{
    close();
}

/*public */tgs::TGSError ASDNetworkServer::open(std::string const& port, int thread)
{
    int i;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    if (thread > 0) {
        _responder.reset(new(std::nothrow) Responder(this));
        if (_responder != NULL) {
            try {
                _server.reset(new(std::nothrow) Server("0.0.0.0", port, *_responder, boost::network::http::_reuse_address = true));
                if (_server != NULL) {
                    _thread.reset(new(std::nothrow) boost::thread_group);
                    if (_thread != NULL) {
                        for (i = 0; i < thread; ++i) {
                            try {
                                _thread->create_thread(boost::bind(&Server::run, _server.get()));
                            }
                            catch (std::exception& e) {
                                artsatd::getInstance().log(LOG_EMERG, "thread start error [%s]", e.what());
                                error = tgs::TGSERROR_FAILED;
                                break;
                            }
                        }
                    }
                    else {
                        error = tgs::TGSERROR_NO_MEMORY;
                    }
                }
                else {
                    error = tgs::TGSERROR_NO_MEMORY;
                }
            }
            catch (std::exception& e) {
                artsatd::getInstance().log(LOG_EMERG, "server initialize error [%s]", e.what());
                error = tgs::TGSERROR_FAILED;
            }
        }
        else {
            error = tgs::TGSERROR_NO_MEMORY;
        }
        if (error != tgs::TGSERROR_OK) {
            close();
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */void ASDNetworkServer::close(void)
{
    if (_server != NULL) {
        try {
            _server->stop();
        }
        catch (std::exception& e) {
            artsatd::getInstance().log(LOG_EMERG, "server terminate error [%s]", e.what());
        }
        if (_thread != NULL) {
            try {
                _thread->join_all();
            }
            catch (...) {
            }
        }
    }
    _thread.reset();
    _server.reset();
    _responder.reset();
    return;
}

/*protected virtual */tgs::TGSError ASDNetworkServer::Notifier::onRequest(std::string const& path, std::map<std::string, std::string>& query, int* status, std::string* response)
{
    *status = 501;
    *response = "501 Not Implemented / ASDNetworkServer::Notifier::onRequest() is an abstract function.";
    return tgs::TGSERROR_OK;
}

/*protected virtual */tgs::TGSError ASDNetworkServer::Notifier::onJsonRpcRequest(std::string const& body, int* status, std::string* response)
{
    *status = 501;
    *response = "501 Not Implemented / ASDNetworkServer::Notifier::onJsonRpcRequest() is an abstract function.";
    return tgs::TGSERROR_OK;
}

/*public */ASDNetworkServer::Responder::Responder(ASDNetworkServer const* param) : _server(param)
{
}

/*public */ASDNetworkServer::Responder::~Responder(void)
{
}

/*public */void ASDNetworkServer::Responder::operator()(Server::request const& request, Server::response& response)
{
    boost::network::uri::uri uri;
    std::map<std::string, std::string> map;
    std::map<std::string, std::string> query;
    std::map<std::string, std::string>::const_iterator it;
    std::string path;
    int status;
    std::string content;
    tgs::TGSError error;
    
    artsatd::getInstance().log(LOG_NOTICE, "server request [%s %s from %s]", request.method.c_str(), request.destination.c_str(), request.source.c_str());
    if (_server->getNotifier() != NULL) {
        if (request.method == "GET" || request.method == "POST") {
            uri = "http://localhost" + request.destination;
            if (uri.is_valid()) {
                boost::network::uri::query_map_mod(uri, map);
                for (it = map.begin(); it != map.end(); ++it) {
                    query.insert(std::map<std::string, std::string>::value_type(
                        boost::network::uri::decoded(boost::algorithm::replace_all_copy(it->first, "+", " ")),
                        boost::network::uri::decoded(boost::algorithm::replace_all_copy(it->second, "+", " "))
                    ));
                }
                path = boost::network::uri::decoded_path(uri);
                try {
                    if (request.method == "POST" && request.destination == "/rpc.json") {
                        error = _server->getNotifier()->onJsonRpcRequest(request.body, &status, &content);
                    }
                    else {
                        error = _server->getNotifier()->onRequest(path, query, &status, &content);
                    }
                }
                catch (...) {
                    error = tgs::TGSERROR_FAILED;
                }
                if (error == tgs::TGSERROR_OK) {
                    response = Server::response::stock_reply(static_cast<Server::response::status_type>(status), content);
                }
                else {
                    response = Server::response::stock_reply(Server::response::internal_server_error);
                }
            }
            else {
                response = Server::response::stock_reply(Server::response::internal_server_error);
            }
        }
        else {
            response = Server::response::stock_reply(Server::response::not_supported);
        }
    }
    else {
        response = Server::response::stock_reply(Server::response::not_implemented);
    }
    return;
}

/*public */void ASDNetworkServer::Responder::log(Server::string_type const& info)
{
    artsatd::getInstance().log(LOG_ERR, "server error [%s]", info.c_str());
    return;
}
