
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

int main() {
    using namespace nodec;

    logging::record_handlers().connect(logging::record_to_stdout_handler);

    logging::InfoStream(__FILE__, __LINE__) << "Start";

    try {
        //logging::InfoStream(__FILE__, __LINE__) << std::thread::hardware_concurrency();

        logging::InfoStream(__FILE__, __LINE__) << "Main thread id: " << std::this_thread::get_id();

        concurrent::ThreadPoolExecutor executor;

        logging::InfoStream(__FILE__, __LINE__) << executor.thread_count();

        //counter(5);
        {
            auto future = executor.submit(counter, 5);
        }
        {

            auto future = executor.submit(counter, 5);
        }

        {
            auto future = executor.submit([]() {
                logging::InfoStream(__FILE__, __LINE__) << std::this_thread::get_id();
                counter(10);
                return std::this_thread::get_id();
                                          });

            logging::InfoStream(__FILE__, __LINE__) << future.get();
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