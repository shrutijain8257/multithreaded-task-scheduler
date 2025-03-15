#include "ThreadPool.h"
#include <thread>
#include <chrono>

int main() {
    ThreadPool pool(3);

    auto sleep_ms = [](int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    };

    // submit ALL tasks first — queue fills up before any worker touches it
    pool.submit("Analytics report",   Priority::LOW,    [&]{ sleep_ms(200); });
    pool.submit("Payment processor",  Priority::HIGH,   [&]{ sleep_ms(150); });
    pool.submit("Email sender",       Priority::MEDIUM, [&]{ sleep_ms(100); });
    pool.submit("DB backup",          Priority::LOW,    [&]{ sleep_ms(180); });
    pool.submit("Auth token refresh", Priority::HIGH,   [&]{ sleep_ms(80);  });

    // NOW release workers — they see the full queue and pick by priority
    pool.start();

    sleep_ms(800);
    pool.stop();
    return 0;
}