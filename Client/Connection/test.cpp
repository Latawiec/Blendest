#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/signals2.hpp>

#include <iostream>
namespace net       = boost::asio;
namespace beast     = boost::beast;
namespace websocket = beast::websocket;
using namespace std::chrono_literals;

struct BackgroundWs {
    boost::signals2::signal<void(std::string)> callback;

    BackgroundWs(std::string host, uint16_t port)
        : _host(std::move(host))
        , _port(std::to_string(port)) {}

    void Start() {
        net::post(_ws.get_executor(), [this] {
            _stop_requested = false;
            do_step_machine();
        });
    }
    void Stop() { do_stop().get(); }

    ~BackgroundWs() {
        do_stop().wait(); // noexcept, please
        _ioc.stop();
        _ioc.join();
        std::cout << "Hello! I'm dying!" << std::endl;
    }

  private:
    using tcp = net::ip::tcp;
    using error_code = boost::system::error_code;

#define OKAY false
#if OKAY
    struct CallableWrapped {
        std::function<void()> fun;

        CallableWrapped() {
            fun = []{ std::cout << "Who calls me!"; };
        }
        ~CallableWrapped() {
            std::cout << "Callable is absolutely dead." << std::endl;
        }

        void operator()() {
            fun();
        }
    } testCallback; 
#endif 
    const std::string              _host, _port;
    beast::flat_buffer             _buffer;
    net::thread_pool               _ioc{1};
    websocket::stream<tcp::socket> _ws{_ioc};
    net::steady_timer              _timer{_ws.get_executor()};

#if !OKAY
    struct CallableWrapped {
        std::function<void()> fun;

        CallableWrapped() {
            fun = []{ std::cout << "Who calls me!"; };
        }
        ~CallableWrapped() {
            std::cout << "Callable is absolutely dead." << std::endl;
        }

        void operator()() {
            fun();
        }
    } testCallback; 
#endif 

    bool _stop_requested = false;
    enum class Status { INIT, CONNECTED, DISCONNECTED, STOPPED } _status{};

    void handle(error_code ec) {
        std::cerr << "Error: " << ec.message() << std::endl;
        if (ec.failed())
            _status = _stop_requested ? Status::STOPPED : Status::DISCONNECTED;
        do_step_machine();
    }

    void do_step_machine() {
        switch(_status) {
            case Status::INIT:         return do_connect();
            case Status::CONNECTED:    return do_read();
            case Status::DISCONNECTED: return do_reconnect_delay(50ms);
            case Status::STOPPED:      break;
        };
    }

    void do_reconnect_delay(std::chrono::steady_clock::duration d) {
        _timer.expires_after(d);
        _timer.async_wait([this](error_code ec) {
            if (ec) return handle(ec);
            _status = Status::INIT;
            do_step_machine();
        });
    }

    std::future<void> do_stop() {
        return dispatch(_ws.get_executor(), std::packaged_task<void()>([this] {
                            _stop_requested = true;
                            _ws.next_layer().cancel();
                            _ws.close(websocket::normal);
                        }));
    }

    void do_connect() {
        async_connect( //
            _ws.next_layer(), tcp::resolver(_ioc).resolve(_host, _port),
            [this](error_code ec, tcp::endpoint /*ep*/) {
                if (ec) return handle(ec);

                _ws.set_option(websocket::stream_base::decorator([](websocket::request_type& req) {
                    req.set(beast::http::field::user_agent,
                            BOOST_BEAST_VERSION_STRING " WsConnect");
                }));

                _ws.async_handshake(_host + ':' + _port, "/", [this](error_code ec) {
                    if (ec) return handle(ec);
                    _status = Status::CONNECTED;
                    _buffer.clear();
                    do_step_machine();
                });
            });
    }

    void do_read() {
        _ws.async_read(_buffer, [this](error_code ec, size_t) {
            std::cout << std::endl << "One more read please!" << std::endl;
            testCallback();
            if (ec) return handle(ec);

            if (_ws.is_message_done()) {
                callback(beast::buffers_to_string(_buffer.cdata()));
                _buffer.clear();
                do_step_machine();
            } else {
                do_read();
            }
        });
    }
};

void handle_message(std::string_view msg) { std::cout << "Handling " << msg << std::flush; }

int main() {
    {
        BackgroundWs client("localhost", 8080);
        boost::signals2::scoped_connection subscription = client.callback.connect(handle_message);

        client.Start();

        std::string input;
        while (std::cout << "Enter 'Stop' to stop... " && getline(std::cin, input))
            if (input == "Stop") break;

        // client.Stop(); // or just rely on destructor
    } // destructor

    std::cout << "Press Enter to quit... ";
    std::cin.ignore(1024, '\n');
}