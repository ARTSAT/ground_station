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
        key   =  spirit::qi::char_("a-zA-Z_") >> *spirit::qi::char_("a-zA-Z_0-9/%");
        value = *spirit::qi::char_("a-zA-Z_0-9/%+*-.");
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
        static  void                    getURI          (Server::request::string_type const& destination, boost::network::uri::uri* result);
        static  void                    getPath         (boost::network::uri::uri const& uri, std::string* result);
        static  void                    getQuery        (boost::network::uri::uri const& uri, insensitive::map<std::string, std::string>* result);
        static  void                    getCookie       (Server::request::vector_type const& header, insensitive::map<std::string, std::string>* result);
        static  void                    mergeHeader     (insensitive::map<std::string, std::string> const& header, Server::response::headers_vector* result);
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
                _server.reset(new(std::nothrow) Server(Server::options(*_responder).address("0.0.0.0").port(port).reuse_address(true)));
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

/*public static */void ASDNetworkServer::Notifier::replyStatus(Server::response::status_type code, ResponseRec* response)
{
    Server::response stock;
    Server::response::headers_vector::const_iterator it;
    
    if (response != NULL) {
        stock = Server::response::stock_reply(code);
        response->status = stock.status;
        for (it = stock.headers.begin(); it != stock.headers.end(); ++it) {
            if (!boost::iequals(it->name, "Content-Length")) {
                response->header[it->name] = it->value;
            }
        }
        response->content = stock.content;
    }
    return;
}

/*protected virtual */tgs::TGSError ASDNetworkServer::Notifier::onRequest(RequestRec const& request, ResponseRec* response)
{
    replyStatus(Server::response::not_implemented, response);
    return tgs::TGSERROR_OK;
}

/*private static */void ASDNetworkServer::Notifier::setCookie(std::string const& name, std::string const& value, unsigned int const* age, ir::IRXTime const* expire, std::string const& domain, std::string const& path, bool secure, ResponseRec* response)
{
    std::string string;
    
    if (!name.empty() && response != NULL) {
        string  = boost::replace_all_copy(boost::network::uri::encoded(name), "%20", "+");
        string += "=";
        string += boost::replace_all_copy(boost::network::uri::encoded(value), "%20", "+");
        if (age != NULL) {
            string += "; max-age=";
            string += boost::lexical_cast<std::string>(*age);
        }
        if (expire != NULL) {
            string += "; expires=";
            string += expire->format("%Wek, %DD-%Mth-%YYYY %hh:%mm:%ss GMT");
        }
        if (!domain.empty()) {
            string += "; domain=";
            string += domain;
        }
        if (!path.empty()) {
            string += "; path=";
            string += path;
        }
        if (secure) {
            string += "; secure";
        }
        string += ";";
        response->header["Set-Cookie:" + name] = string;
    }
    return;
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
    Notifier::RequestRec input;
    Notifier::ResponseRec output;
    tgs::TGSError error;
    
    if (_server->getNotifier() != NULL) {
        if (boost::iequals(method(request), "GET") || boost::iequals(method(request), "POST")) {
            getURI(destination(request), &uri);
            if (uri.is_valid()) {
                input.host = source(request);
                getPath(uri, &input.path);
                getQuery(uri, &input.query);
                getCookie(headers(request), &input.cookie);
                input.content = body(request);
                output.status = Server::response::ok;
                output.header["Content-Type"] = "text/plain";
                try {
                    error = _server->getNotifier()->onRequest(input, &output);
                }
                catch (...) {
                    error = tgs::TGSERROR_FAILED;
                }
                if (error == tgs::TGSERROR_OK) {
                    response = Server::response::stock_reply(static_cast<Server::response::status_type>(output.status), output.content);
                    mergeHeader(output.header, &response.headers);
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
    artsatd::getInstance().log(LOG_ERR, "server internal error [%s]", info.c_str());
    return;
}

/*private static */void ASDNetworkServer::Responder::getURI(Server::request::string_type const& destination, boost::network::uri::uri* result)
{
    *result = boost::network::uri::uri();
    *result << boost::network::uri::scheme("http") << boost::network::uri::host("localhost") << boost::network::uri::path(destination);
    return;
}

/*private static */void ASDNetworkServer::Responder::getPath(boost::network::uri::uri const& uri, std::string* result)
{
    *result = boost::network::uri::decoded_path(uri);
    return;
}

/*private static */void ASDNetworkServer::Responder::getQuery(boost::network::uri::uri const& uri, insensitive::map<std::string, std::string>* result)
{
    insensitive::map<std::string, std::string> query;
    insensitive::map<std::string, std::string>::const_iterator it;
    
    boost::network::uri::query_map_mod(uri, query);
    result->clear();
    for (it = query.begin(); it != query.end(); ++it) {
        (*result)[boost::network::uri::decoded(boost::replace_all_copy(it->first, "+", "%20"))] = boost::network::uri::decoded(boost::replace_all_copy(it->second, "+", "%20"));
    }
    return;
}

/*private static */void ASDNetworkServer::Responder::getCookie(Server::request::vector_type const& header, insensitive::map<std::string, std::string>* result)
{
    Server::request::vector_type::const_iterator hit;
    std::vector<std::string> cookie;
    std::vector<std::string>::iterator cit;
    int index;
    
    result->clear();
    for (hit = header.begin(); hit != header.end(); ++hit) {
        if (boost::iequals(hit->name, "Cookie")) {
            boost::split(cookie, hit->value, boost::is_any_of(";"));
            for (cit = cookie.begin(); cit != cookie.end(); ++cit) {
                boost::trim(*cit);
                if ((index = cit->find("=")) != std::string::npos) {
                    (*result)[boost::network::uri::decoded(boost::replace_all_copy(cit->substr(0, index), "+", "%20"))] = boost::network::uri::decoded(boost::replace_all_copy(cit->substr(index + 1), "+", "%20"));
                }
            }
        }
    }
    return;
}

/*private static */void ASDNetworkServer::Responder::mergeHeader(insensitive::map<std::string, std::string> const& header, Server::response::headers_vector* result)
{
    Server::response::headers_vector::iterator hit;
    insensitive::map<std::string, std::string> copy;
    insensitive::map<std::string, std::string>::const_iterator cit;
    
    copy = header;
    for (hit = result->begin(); hit != result->end(); ++hit) {
        if (!boost::iequals(hit->name, "Set-Cookie")) {
            if ((cit = copy.find(hit->name)) != copy.end()) {
                hit->value = cit->second;
                copy.erase(cit);
            }
        }
    }
    for (cit = copy.begin(); cit != copy.end(); ++cit) {
        result->push_back(Server::response_header());
        result->back().name = cit->first.substr(0, cit->first.find(":"));
        result->back().value = cit->second;
    }
    return;
}
