#include <nodec/logging.hpp>
#include <nodec/endian.hpp>

int main() {
    using namespace nodec;
    using namespace nodec::endian;


    logging::record_handlers().connect(logging::record_to_stdout_handler);

    try {
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint16_t in = 0x0102;
            uint16_t out;
            internal::little_endian_conv<uint16_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::little_endian_conv<uint16_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            int16_t in = -100;
            int16_t out;

            internal::little_endian_conv<int16_t>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::little_endian_conv<int16_t>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint32_t in = 0x01020304;
            uint32_t out;
            internal::little_endian_conv<uint32_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::little_endian_conv<uint32_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            float in = 3.14;
            float out;
            internal::little_endian_conv<float>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::little_endian_conv<float>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint64_t in = 0x0102030405060708;
            uint64_t out;
            internal::little_endian_conv<uint64_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::little_endian_conv<uint64_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            double in = 3.14;
            double out;
            internal::little_endian_conv<double>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::little_endian_conv<double>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint64_t in, out;
            in = 0x0102030405060708;
            out = from_little_endian(in);
            info << std::hex << in << " -> " << out << "\n";

            in = to_little_endian(out);
            info << std::hex << out << " -> " << in << "\n";
        }

        // --- 

        logging::InfoStream(__FILE__, __LINE__) << "--- big endian ---";

        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint16_t in = 0x0102;
            uint16_t out;
            internal::big_endian_conv<uint16_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::big_endian_conv<uint16_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            int16_t in = -100;
            int16_t out;

            internal::big_endian_conv<int16_t>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::big_endian_conv<int16_t>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint32_t in = 0x01020304;
            uint32_t out;
            internal::big_endian_conv<uint32_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::big_endian_conv<uint32_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            float in = 3.14;
            float out;
            internal::big_endian_conv<float>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::big_endian_conv<float>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint64_t in = 0x0102030405060708;
            uint64_t out;
            internal::big_endian_conv<uint64_t>::from(in, out);
            info << std::hex << in << " -> " << out << "\n";

            internal::big_endian_conv<uint64_t>::to(out, in);
            info << std::hex << out << " -> " << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            double in = 3.14;
            double out;
            internal::big_endian_conv<double>::from(in, out);
            info << std::dec << in << ":" << std::hex << in << " -> " << std::dec << out << ":" << std::hex << out << "\n";

            internal::big_endian_conv<double>::to(out, in);
            info << std::dec << out << ":" << std::hex << out << " -> " << std::dec << in << ":" << std::hex << in << "\n";
        }
        {
            logging::InfoStream info(__FILE__, __LINE__);
            info << "\n";
            uint64_t in, out;
            in = 0x0102030405060708;
            out = from_big_endian(in);
            info << std::hex << in << " -> " << out << "\n";

            in = to_big_endian(out);
            info << std::hex << out << " -> " << in << "\n";
        }

    }
    catch (std::exception& e) {
        logging::ErrorStream(__FILE__, __LINE__) << e.what();
    }

    return 0;
}