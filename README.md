# DAC8532

DAC8532 library for RaspberryPi. Requires BCM2835 library (which is compatible with BCM2836 and BCM2837 chips, so it should work under RPi2, 3 and Zero as well).
BCM2835 library can be downloaded here: http://www.airspayce.com/mikem/bcm2835/

## Usage

References (very poorly generated, i'd recommend looking at docs in .hpp file) can be found in Wiki.
Compile with `g++ -c DAC8532.cpp -lbcm2835` and link with your code. Or just compile everything at whole. Maybe i'll provide makefile soon.
To be honest, provided example should be enough. If not sure, read comments inside .hpp and you should be fine.

## Example

```cpp
#include "DAC8532.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

int main() {
    DAC8532 dac(23);

    try {
        dac.initialize();
    } catch (BCMInitException e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    dac.set_power_down_mode(DAC8532::Channel::A, DAC8532::PowerDownMode::None);
    dac.set_power_down_mode(DAC8532::Channel::B, DAC8532::PowerDownMode::None);

    for(double i = 0.; i < 5.; i += 0.1) {
        dac.write_voltage(DAC8532::Channel::A, i);
        dac.write_voltage(DAC8532::Channel::B, 5. - i);

        auto info_a = dac.get_channel_info(DAC8532::Channel::A);
        auto info_b = dac.get_channel_info(DAC8532::Channel::B);
        std::cout << "A voltage: " << info_a.voltage << ", B voltage: " << info_b.voltage << '\n';

        std::this_thread::sleep_for(50ms);
    }

    dac.set_power_down_mode(DAC8532::Channel::A, DAC8532::PowerDownMode::HighImpedance);

    for(double i = 0.; i < 5.; i += 0.1) {
        dac.write_voltage(DAC8532::Channel::B, i);
        dac.write_voltage(DAC8532::Channel::A, 5. - i);

        auto info_a = dac.get_channel_info(DAC8532::Channel::A);
        auto info_b = dac.get_channel_info(DAC8532::Channel::B);
        std::cout << "A voltage: " << info_a.voltage << ", B voltage: " << info_b.voltage << '\n';

        std::this_thread::sleep_for(50ms);
    }

    dac.cleanup();
}
```