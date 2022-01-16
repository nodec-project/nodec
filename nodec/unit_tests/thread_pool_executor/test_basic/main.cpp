
#include <nodec/logging.hpp>
#include <nodec/concurrent/thread_pool_executor.hpp>

#include <iostream>

std::string counter(int timeout) {
    while (timeout > 0) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << std::this_thread::get_id() << ">>> " << timeout << std::endl;
        //std::cout << std::this_thread::get_id() << ">>> " << timeout << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        --timeout;
    }
    nodec::logging::InfoStream(__FILE__, __LINE__) << std::this_thread::get_id() << ">>> Done!" << std::endl;
    //std::cout << std::this_thread::get_id() << ">>> Done!" << std::endl;

    return { "OK!!" };
}

class HelloWorld {
public:
    std::string say_hello(int number) {
        std::cout << "[say_hello (" << std::this_thread::get_id() << ")] >>> Start" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        std::ostringstream oss;
        oss << "[say_hello (" << std::this_thread::get_id() << ")] >>> Hello! number: " << number;
        std::cout << "[say_hello (" << std::this_thread::get_id() << ")] >>> End" << std::endl;
        return oss.str();
    }
};

std::string say_ok(int number) {
    std::cout << "[say_ok (" << std::this_thread::get_id() << ")] >>> Start" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::ostringstream oss;
    oss << "[say_ok (" << std::this_thread::get_id() << ")] >>> OK! number: " << number;
    std::cout << "[say_ok (" << std::this_thread::get_id() << ")] >>> End" << std::endl;
    return oss.str();
}

int main() {
    using namespace nodec;

    {
        concurrent::ThreadPoolExecutor executor;

        std::cout << "thread_count: " << executor.thread_count() << std::endl;

        auto ok_future = executor.submit(say_ok, 100);

        HelloWorld hello;
        auto hello_future = executor.submit([&](auto number){ return hello.say_hello(number); }, 999);

        std::cout << ok_future.get() << std::endl;
        std::cout << hello_future.get() << std::endl;

        return 0;
    }

    logging::record_handlers().connect(logging::record_to_stdout_handler);

    logging::InfoStream(__FILE__, __LINE__) << "Start";

    try {
        //logging::InfoStream(__FILE__, __LINE__) << std::thread::hardware_concurrency();

        logging::InfoStream(__FILE__, __LINE__) << "Main thread id: " << std::this_thread::get_id();

        concurrent::ThreadPoolExecutor executor;

        logging::InfoStream(__FILE__, __LINE__) << executor.thread_count();

        //counter(5);
        {
            auto future = executor.submit(counter, 10);
        }
        {

            auto future = executor.submit(counter, 5);
            logging::InfoStream(__FILE__, __LINE__) << future.get();
        }

        {
            auto future = executor.submit([&]() {
                logging::InfoStream(__FILE__, __LINE__) << std::this_thread::get_id();
                //executor.submit(counter, 12);
                counter(3);
                return std::this_thread::get_id();
                });

            //logging::InfoStream(__FILE__, __LINE__) << future.get();
        }

        //std::this_thread::yield();

        //std::this_thread::sleep_for()
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    catch (std::exception& e) {
        logging::FatalStream(__FILE__, __LINE__) << e.what();
    }

    return 0;
}