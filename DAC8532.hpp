//! DAC8532 library for RaspberryPi
/*!
    This library allows to easely use DAC8532.
    I/O is based on BCM2835 library, it should work well on any RPi and microcomputer with this chip (or BCM2836 and BCM2837)
    The library have to be installed separately, source is available here: http://www.airspayce.com/mikem/bcm2835/
*/

#ifndef DAC8532_HPP
#define DAC8532_HPP
#include <bcm2835.h>
#include <cinttypes> // For precisely sized types
#include "BCMException.hpp"

class DAC8532 {

    //! Structure containing bits for easy flag calculation
    struct Bits {
        static constexpr uint8_t LOAD_B = 0b00100000;
        static constexpr uint8_t LOAD_A = 0b00010000;
        static constexpr uint8_t BUFFER_SELECT_A = 0b00000000;
        static constexpr uint8_t BUFFER_SELECT_B = 0b00000100;
        static constexpr uint8_t POWER_DOWN_1 = 0b00000010;
        static constexpr uint8_t POWER_DOWN_0 = 0b00000001;
    };

public:

    //! Enumeration for channel used. AB is for both channels
    enum class Channel {
        A, /*!< Channel A */
        B  /*!< Channel B */
    };

    //! Enumeration for setting power mode
    enum class PowerDownMode : uint8_t {
        None = 0, /*!< No power down mode - channel working normally */
        OutputTo1k = Bits::POWER_DOWN_0, /*!< Output connected to 1k resistance */
        OutputTo100k = Bits::POWER_DOWN_1, /*!< Output connected to 100k resistance */
        HighImpedance = Bits::POWER_DOWN_0 | Bits::POWER_DOWN_1 /*!< Output disconnected, high impedance mode */
    };

    //! Structure containing information (voltage and power down mode) for channel
    struct ChannelInfo {
    public:
        Channel channel; /*!< Used to identify which channel we are looking at */
        double voltage; /*!< Voltage set on channel at the moment */
        PowerDownMode power_down_mode; /*!< Power mode of channel */
    };

    //! Constructor
    /*!
        \param chip_select DAC chip select pin
        \param reference_voltage Reference voltage of DAC (5V by default)
    */
    DAC8532(uint8_t chip_select, double reference_voltage = 5.f);

    //! Function initializing the DAC - have to be called BEFORE any operations. Can throw BCMInitException.
    /*!
            \param clock_divider clock divider for SPI bus. 8 is a safe value, bus will run at 15.625MHz under RPi2 and 25MHz under RPi3 with this divider. For detailed info, look here: http://www.airspayce.com/mikem/bcm2835/group__constants.html#gaf2e0ca069b8caef24602a02e8a00884e
    */
    void initialize(uint16_t clock_divider = 16);

    //! Set reference voltage for DAC
    /*!
        \param reference_voltage reference voltage
    */
    void set_reference_voltage(double reference_voltage);

    //! Get reference voltage of DAC
    /*!
        \return Reference voltage
    */
    double reference_voltage() const;

    //! Set voltage on selected channel. Ensure that channel is not in power down mode if you want to see results.
    /*!
        \param channel which channel you are writing
        \param voltage voltage to be set
    */
    void write_voltage(Channel channel, double voltage);
    
    //! Gets information about channel
    /*!
        \param channel which channel you want info about
        \return ChannelInfo structure containing the channel data
    */
    ChannelInfo get_channel_info(Channel channel) const;

    //! Sets power down mode for channel
    /*!
        \param channel channel to modify
        \param power_down_mode power down mode to be set
    */
    void set_power_down_mode(Channel channel, PowerDownMode power_down_mode);

private:
    uint8_t m_cs{};
    double m_ref_v{};
    ChannelInfo m_channel_a, m_channel_b;

    void enable_dac() const;
    void disable_dac() const;
    uint16_t convert_voltage(double voltage) const;

    void write_raw_data(uint8_t control_bits, uint16_t data) const;
};

#endif