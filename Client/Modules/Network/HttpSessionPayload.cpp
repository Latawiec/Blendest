#include "HttpSessionPayload.hpp"


#include <boost/asio/strand.hpp>
#include <iostream>
#include <fstream>

namespace Network {

HttpSessionPayload::HttpSessionPayload(const std::string& host, const std::string& port)
: _host(host)
, _port(port)
{}

HttpSessionPayload::~HttpSessionPayload()
{
    Stop();
}

void HttpSessionPayload::Start()
{
    boost::asio::post(_stream.get_executor(), [this] {
        _stop_requested = false;
        do_connect();
    });
}

void HttpSessionPayload::Stop()
{
    do_stop().wait();
}

std::future<Error> HttpSessionPayload::GetFile(const std::string& target, const std::string& outputFilePath)
{
    return do_getFile(target, outputFilePath);
}

std::future<Error> HttpSessionPayload::SendFile(const std::string& target, const std::string& inputFilePath)
{
    return do_sendFile(target, inputFilePath);
}

void HttpSessionPayload::do_connect()
{
    using tcp = boost::asio::ip::tcp;
    using error_code = boost::system::error_code;
    using namespace boost;

    _stream.async_connect(tcp::resolver(_ioc).resolve(_host, _port),
        [this](error_code ec, tcp::endpoint /*ep*/) {
            if (ec) {
                const std::string err = ec.message();
                return;
            }

            // Anything else here?
        }
    );
}

std::future<void> HttpSessionPayload::do_stop() {
    using namespace boost;
    return dispatch(_stream.get_executor(), std::packaged_task<void()>([this] {
        _stop_requested = true;
        _stream.cancel();
    }));
}

std::future<Error> HttpSessionPayload::do_getFile(const std::string& target, const std::string& outputFilePath)
{
    using namespace boost;
    std::packaged_task<Error()> task(
        std::bind([this](const std::string target, const std::string outputFilePath, const std::string host) -> Error {
            beast::error_code  ec;
            beast::http::request<beast::http::empty_body> req;
            beast::http::response<beast::http::file_body> res;

            try {

                req.method(beast::http::verb::get);
                req.target(target);
                req.set(beast::http::field::host, host);
                req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                write(_stream, req);

                res.body().open(outputFilePath.c_str(), boost::beast::file_mode::write, ec);

                beast::flat_buffer buffer;
                beast::http::response_parser<beast::http::file_body> parser(std::move(res));
                if (ec) {
                    return {
                        .code = ec.value(),
                        .message = ec.message()
                    };
                }
                parser.body_limit(std::numeric_limits<std::uint64_t>::max());
                beast::http::read(_stream, buffer, parser);

                auto resCode = parser.get().result_int();
                resCode = resCode == 200 ? 0 : resCode;
                return {
                    .code = static_cast<int>(resCode),
                    .message = parser.get().reason() 
                };
            } catch (std::exception e) {
                return {
                    .code = ec.value(),
                    .message = ec.message()
                };
            }

        }, target, outputFilePath, _host));

    return dispatch(_stream.get_executor(), std::move(task));
}

std::vector<BYTE> readFile(const char* filename)
{
    // open the file:
    std::basic_ifstream<BYTE> file(filename, std::ios::binary);

    // read the data:
    return std::vector<BYTE>((std::istreambuf_iterator<BYTE>(file)),
                              std::istreambuf_iterator<BYTE>());
}

std::future<Error> HttpSessionPayload::do_sendFile(const std::string& target, const std::string& inputFilePath)
{
    using namespace boost;
    std::packaged_task<Error()> task(
        std::bind([this](const std::string target, const std::string inputFilePath, const std::string host) -> Error {
            beast::error_code  ec;
            beast::http::request<beast::http::file_body> req;
            beast::http::response<beast::http::file_body> res;

            try {
                req.version(11);
                req.method(beast::http::verb::post);
                req.target(target);
                req.set(beast::http::field::host, host);
                req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
                req.body().open(inputFilePath.c_str(), beast::file_mode::scan, ec);
                req.prepare_payload();

                write(_stream, req);

                return {
                    .code = 0,
                    .message = "" 
                };
            } catch (std::exception e) {
                return {
                    .code = ec.value(),
                    .message = ec.message()
                };
            }

        }, target, inputFilePath, _host));

    return dispatch(_stream.get_executor(), std::move(task));
}


}