#include "DAC8532.hpp"

DAC8532::DAC8532(uint8_t chip_select, double reference_voltage) :
    m_cs{chip_select}, m_ref_v{reference_voltage}
{}

void DAC8532::initialize(uint16_t clock_divider) {
    if (!bcm2835_init()) throw BCMInitException();
    bcm2835_spi_begin();
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
    bcm2835_spi_setClockDivider(clock_divider);
    bcm2835_gpio_fsel(m_cs, BCM2835_GPIO_FSEL_OUTP);

    m_channel_a.channel = Channel::A;
    m_channel_b.channel = Channel::B;

    set_power_down_mode(Channel::A, PowerDownMode::HighImpedance);
    set_power_down_mode(Channel::B, PowerDownMode::HighImpedance);
    disable_dac();
}

void DAC8532::set_reference_voltage(double reference_voltage) {
    m_ref_v = reference_voltage;
}

double DAC8532::reference_voltage() const {
    return m_ref_v;
}

void DAC8532::write_voltage(Channel channel, double voltage) {
    uint16_t data = convert_voltage(voltage);
    uint8_t control_bits{0};
    if (channel == Channel::A) {
        control_bits |= Bits::LOAD_A | Bits::BUFFER_SELECT_A | static_cast<uint8_t>(m_channel_a.power_down_mode);
        m_channel_a.voltage = voltage;
    } else if (channel == Channel::B) {
        control_bits |= Bits::LOAD_B | Bits::BUFFER_SELECT_B | static_cast<uint8_t>(m_channel_b.power_down_mode);
        m_channel_b.voltage = voltage;
    }
    
    enable_dac();
    write_raw_data(control_bits, data);
    disable_dac();
}

DAC8532::ChannelInfo DAC8532::get_channel_info(Channel channel) const {
    if (channel == Channel::A) {
        return m_channel_a;
    } else if (channel == Channel::B) {
        return m_channel_b;
    }
}

void DAC8532::set_power_down_mode(Channel channel, PowerDownMode power_down_mode) {
    uint8_t control_bits{0};
    if (channel == Channel::A) {
        control_bits |= Bits::BUFFER_SELECT_A;
        m_channel_a.power_down_mode = power_down_mode;
    } else if (channel == Channel::B) {
        control_bits |= Bits::BUFFER_SELECT_B;
        m_channel_b.power_down_mode = power_down_mode;
    }

    control_bits |= static_cast<uint8_t>(power_down_mode);

    enable_dac();
    write_raw_data(control_bits, 0);
    disable_dac();
}

bool DAC8532::cleanup() const {
    bcm2835_spi_end();
    return static_cast<bool>(bcm2835_close());
}

void DAC8532::enable_dac() const {
    bcm2835_gpio_write(m_cs, LOW);
}

void DAC8532::disable_dac() const {
    bcm2835_gpio_write(m_cs, HIGH);
}

uint16_t DAC8532::convert_voltage(double voltage) const {
    return static_cast<uint16_t>(voltage * 65536.f / m_ref_v);
}

void DAC8532::write_raw_data(uint8_t control_bits, uint16_t data) const {
    bcm2835_spi_transfer(control_bits);
    bcm2835_spi_transfer(static_cast<uint8_t>(data >> 8));
    bcm2835_spi_transfer(static_cast<uint8_t>(data & 0xFF));
}