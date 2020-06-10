// Standard library
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

// JSON library
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Utilities
#include "expected.hpp"
#include "mtry.hpp"
#include "trim.hpp"

// Our reactive stream implementation
#include "filter.hpp"
// #include "join.hpp"
#include "sink.hpp"
#include "transform.hpp"
// #include "values.hpp"

// Service implementation
#include "service.hpp"

/**
 * For funcitons that return json objects, but which can fail
 * and return an exception
 */
using expected_json = expected<json, std::exception_ptr>;

/**
 * Basic bookmark information. Holds an URL and a title
 */
struct bookmark_t {
    std::string url;
    std::string text;
};

/**
 * Creates a string from the bookmark info in the following format:
 * [text](url)
 */
std::string to_string(const bookmark_t &page) {
    return "[" + page.text + "](" + page.url + ")";
}

/**
 * Writes the bookmark info in the following format:
 * [text](url)
 */
std::ostream &operator<<(std::ostream &out, const bookmark_t &page) {
    return out << "[" << page.text << "](" << page.url << ")";
}

/**
 * Type that contains a bookmark or an error (an exception)
 */
using expected_bookmark = expected<bookmark_t, std::exception_ptr>;

/**
 * Tries to read the bookmark info from JSON. In the case
 * of failure, it will return an exception.
 */
expected_bookmark bookmark_from_json(const json &data) {
    return mtry([&] {
        return bookmark_t{data.at("FirstURL"), data.at("Text")};
    });
}



std::string WithTime(const std::string &msg) {
    std::time_t t = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "[%Y-%m-%d %H:%M:%S]");
    return ss.str() + " " + msg;
}

int main() {
    using namespace reactive::operators;

    // We are lifting the transform and filter functions
    // to work on the with_client<T> type that adds the
    // socket information to the given value so that we
    // can reply to the client
    auto transform = [](auto f) {
        return reactive::operators::transform(lift_with_client(f));
    };
    auto filter = [](auto f) {
        return reactive::operators::filter(apply_with_client(f));
    };

    boost::asio::io_service event_loop;
    unsigned short port = 42042;

    // clang-format off
    auto pipeline = service(event_loop, port)
        | transform(trim)
        // Ignoring comments and empty messages
        | filter([](const std::string &message) {
                     return message.length() > 0 && message[0] != '#';
                 })
        // Trying to parse the input
        | transform([](const std::string &message) {
                        return mtry([&] {
                            return json::parse(message);
                        });
                    })
        // Converting the result into the bookmark
        | transform([](const auto &exp) {
                        return mbind(exp, bookmark_from_json);
                    })
        | sink([](const auto &message) {
                   const auto exp_bookmark = message.value;

                   if(!exp_bookmark) {
                       std::string msg = WithTime("ERROR: Request was not understood\n");
                       std::cerr << msg;
                       message.reply(msg);
                       return;
                   }

                   if(exp_bookmark->text.find("C++") != std::string::npos) {
                       std::string msg =
                           WithTime("OK: " + to_string(exp_bookmark.get()) + "\n");
                       std::cerr << msg;
                       message.reply(msg);
                   } else {
                       std::string msg = WithTime("ERROR: Not a C++-related link\n");
                       std::cerr << msg;
                       message.reply(msg);
                   }
               });
    // clang-format on

    // Starting the Boost.ASIO service
    std::cerr << WithTime("Service is running at port " + std::to_string(port) + "...\n");
    event_loop.run();
}
