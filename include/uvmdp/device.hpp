#ifndef UVMDP_DEVICE_HPP
#define UVMDP_DEVICE_HPP
#include <cstdint>

namespace benni {
    using address_t = std::uint64_t;
    class Bus;
    class BusDevice {
    private:
    	friend class Bus;
    	Bus* internal_bus{};
    public:
        const address_t bus_size;
        constexpr explicit BusDevice(address_t size) noexcept : bus_size(size) {}
        constexpr virtual ~BusDevice() noexcept = default;

        [[nodiscard]]
        constexpr Bus* bus() const noexcept { return internal_bus; }

        constexpr virtual void bus_write8(address_t addr, std::uint8_t data) noexcept {}
        constexpr virtual void bus_write16(address_t addr, std::uint16_t data) noexcept;
        constexpr virtual void bus_write32(address_t addr, std::uint32_t data) noexcept;
        constexpr virtual void bus_write64(address_t addr, std::uint64_t data) noexcept;

		constexpr virtual std::uint8_t bus_read8(address_t addr) noexcept { return 0; }
		constexpr virtual std::uint16_t bus_read16(address_t addr) noexcept;
		constexpr virtual std::uint32_t bus_read32(address_t addr) noexcept;
		constexpr virtual std::uint64_t bus_read64(address_t addr) noexcept;

		constexpr virtual void clk() noexcept {}
		constexpr virtual void reset() noexcept {}
    };



    constexpr void BusDevice::bus_write16(address_t addr, std::uint16_t data) noexcept {
    	bus_write8(addr + 0, data & 0xff);
    	bus_write8(addr + 1, data >> 8);
    }
    constexpr void BusDevice::bus_write32(address_t addr, std::uint32_t data) noexcept {
    	bus_write16(addr + 0, data & 0xffff);
    	bus_write16(addr + 2, data >> 16);
    }
    constexpr void BusDevice::bus_write64(address_t addr, std::uint64_t data) noexcept {
    	bus_write32(addr + 0, data & 0xffff'ffff);
    	bus_write32(addr + 4, data >> 32);
    }

    constexpr std::uint16_t BusDevice::bus_read16(address_t addr) noexcept {
    	return bus_read8(addr) | (static_cast<std::uint16_t>(bus_read8(addr + 1)) << 8);
    }
    constexpr std::uint32_t BusDevice::bus_read32(address_t addr) noexcept {
    	return bus_read16(addr) | (static_cast<std::uint32_t>(bus_read16(addr + 2)) << 16);
    }
    constexpr std::uint64_t BusDevice::bus_read64(address_t addr) noexcept {
    	return bus_read32(addr) | (static_cast<std::uint64_t>(bus_read32(addr + 4)) << 32);
    }
}

#endif /* UVMDP_DEVICE_HPP */
