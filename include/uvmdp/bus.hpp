#ifndef UVMDP_BUS_HPP
#define UVMDP_BUS_HPP
#include <vector>
#include "device.hpp"

namespace benni {
    class Bus {
    private:
		struct Connection {
			BusDevice* dev{};
			address_t start{}, end{};

			constexpr Connection() noexcept = default;
			[[nodiscard]]
			constexpr Connection(address_t addr, BusDevice* dev) noexcept
				: dev(dev), start(addr), end(addr + dev->bus_size) {}
			constexpr Connection(Connection&& c) noexcept
				: dev(dev), start(c.start), end(c.end) { c.dev = nullptr; }
			constexpr ~Connection() noexcept { delete dev; dev = nullptr; }
			constexpr Connection& operator=(Connection&& c) noexcept {
				delete[] dev;
				dev = c.dev;
				start = c.start;
				end = c.end;
				c.dev = nullptr;
				return *this;
			}
			Connection(const Connection&) = delete;
			Connection& operator=(const Connection&) = delete;

			[[nodiscard]]
			constexpr bool isMapped(address_t addr, address_t n = 1) const noexcept {
				return addr >= start && (addr + n) <= end;
			}
			[[nodiscard]]
			constexpr BusDevice* operator->() const noexcept { return dev; }
		};
		std::vector<Connection> devs{};
    public:
		// unfortunately Bus can't be constexpr because GCC and Clang
		// don't support constepxr std::vector yet (06/24/2020)
		inline Bus() = default;

	    inline Bus& connect(address_t addr, BusDevice* dev);
	    inline void clk() noexcept;
	    inline void reset() noexcept;

		inline void write8(address_t addr, uint8_t data) noexcept;
	    inline void write16(address_t addr, uint16_t data) noexcept;
	    inline void write32(address_t addr, uint32_t data) noexcept;
	    inline void write64(address_t addr, uint64_t data) noexcept;

	    inline uint8_t read8(address_t addr) noexcept;
	    inline uint16_t read16(address_t addr) noexcept;
	    inline uint32_t read32(address_t addr) noexcept;
	    inline uint64_t read64(address_t addr) noexcept;
    };

    inline Bus& Bus::connect(address_t addr, BusDevice *dev) {
	    dev->internal_bus = this;
	    devs.emplace_back(addr, dev);
	    return *this;
    }
    inline void Bus::clk() noexcept {
    	for (const auto& dev : devs) dev->clk();
    }
	inline void Bus::reset() noexcept {
		for (const auto& dev : devs) dev->reset();
	}
    inline void Bus::write8(address_t addr, uint8_t data) noexcept {
    	for (const auto& dev : devs) {
    		if (dev.isMapped(addr)) {
    			dev->bus_write8(addr - dev.start, data);
    			return;
    		}
    	}
    }
    inline void Bus::write16(address_t addr, uint16_t data) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 2)) {
    			dev->bus_write16(a, data);
    			return;
    		}
    		else if (dev.isMapped(addr)) {
    			dev->bus_write8(a, data & 0xff);
    			write8(addr + 1, data >> 8);
    		}
    	}
    }
    inline void Bus::write32(address_t addr, uint32_t data) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 4)) {
    			dev->bus_write32(a, data);
    			return;
    		}
    		else if (dev.isMapped(addr, 3)) {
    			dev->bus_write16(a, data & 0xffff);
    			dev->bus_write8(a + 2, data >> 16);
    			write8(addr + 3, data >> 24);
    			return;
    		}
    		else if (dev.isMapped(addr, 2)) {
    			dev->bus_write16(a, data & 0xffff);
    			write16(addr + 2, data >> 16);
    			return;
    		}
    		else if (dev.isMapped(addr)) {
    			dev->bus_write8(a, data & 0xff);
    			write8(addr + 1, data >> 8);
    			write16(addr + 2, data >> 16);
    			return;
    		}
    	}
    }
    inline void Bus::write64(address_t addr, uint64_t data) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 8)) {
    			dev->bus_write64(a, data);
    			return;
    		}
    		else if (dev.isMapped(addr, 7)) {
    			dev->bus_write32(a, data & 0xffff'ffff);
    			dev->bus_write16(a + 4, data >> 32);
    			dev->bus_write8(a + 6, data >> 48);
    			write8(addr + 7, data >> 56);
    			return;
    		}
		    else if (dev.isMapped(addr, 6)) {
			    dev->bus_write32(a, data & 0xffff'ffff);
			    dev->bus_write16(a + 4, data >> 32);
			    write16(addr + 6, data >> 48);
			    return;
		    }
		    else if (dev.isMapped(addr, 5)) {
			    dev->bus_write32(a, data & 0xffff'ffff);
			    dev->bus_write8(a + 4, data >> 32);
			    write8(a + 5, data >> 40);
			    write16(addr + 6, data >> 48);
			    return;
		    }
		    else if (dev.isMapped(addr, 4)) {
		    	dev->bus_write32(a, data & 0xffff'ffff);
		    	write32(addr + 4, data >> 32);
		    	return;
		    }
		    else if (dev.isMapped(addr, 3)) {
		    	dev->bus_write16(a, data & 0xffff);
		    	dev->bus_write8(a + 2, data >> 16);
		    	write8(addr + 3, data >> 24);
			    write32(addr + 4, data >> 32);
			    return;
		    }
		    else if (dev.isMapped(addr, 2)) {
			    dev->bus_write16(a, data & 0xffff);
			    write16(addr + 2, data >> 16);
			    write32(addr + 4, data >> 32);
			    return;
		    }
		    else if (dev.isMapped(addr)) {
		    	dev->bus_write8(a, data & 0xff);
		    	write8(addr + 1, data >> 8);
		    	write16(addr + 2, data >> 16);
		    	write32(addr + 4, data >> 32);
		    	return;
		    }
    	}
    }

    inline uint8_t Bus::read8(address_t addr) noexcept {
    	for (const auto& dev : devs) {
			if (dev.isMapped(addr)) return dev->bus_read8(addr - dev.start);
    	}
    	return 0;
    }
    inline uint16_t Bus::read16(address_t addr) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 2)) return dev->bus_read16(a);
    		else return dev->bus_read8(a) | (static_cast<uint16_t>(read8(addr + 1)) << 8);
    	}
    	return 0;
    }
    inline uint32_t Bus::read32(address_t addr) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 4)) return dev->bus_read32(a);
    		else if (dev.isMapped(addr, 3)) {
    			uint32_t x = 0;
    			x |= dev->bus_read16(a);
    			x |= dev->bus_read8(a + 2) << 16;
    			x |= read8(addr + 3) << 24;
    			return x;
    		}
    		else if (dev.isMapped(addr, 2))
    			return dev->bus_read16(a) | (static_cast<uint32_t>(read16(addr + 2)) << 16);
    		else if (dev.isMapped(addr)) {
    			uint32_t x = 0;
    			x |= dev->bus_read8(a);
    			x |= read8(addr + 1) << 8;
    			x |= read16(addr + 2) << 16;
    			return x;
    		}
    	}
    	return 0;
    }
    inline uint64_t Bus::read64(address_t addr) noexcept {
    	for (const auto& dev : devs) {
    		const address_t a = addr - dev.start;
    		if (dev.isMapped(addr, 8)) return dev->bus_read64(a);
    		else if (dev.isMapped(addr, 7)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read32(a);
    			x |= (uint64_t)dev->bus_read16(a + 4) << 32;
    			x |= (uint64_t)dev->bus_read8(a + 6) << 48;
    			x |= (uint64_t)read8(addr + 7) << 56;
    			return x;
    		}
    		else if (dev.isMapped(addr, 6)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read32(a);
    			x |= (uint64_t)dev->bus_read16(a + 4) << 32;
    			x |= (uint64_t)read16(addr + 6) << 48;
    			return x;
    		}
    		else if (dev.isMapped(addr, 5)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read32(a);
    			x |= (uint64_t)dev->bus_read8(a + 4) << 32;
    			x |= (uint64_t)read8(a + 5) << 40;
    			x |= (uint64_t)read16(a + 6) << 48;
    			return x;
    		}
    		else if (dev.isMapped(addr, 4))
    			return dev->bus_read32(a) | (static_cast<uint64_t>(read32(addr + 4)) << 32);
    		else if (dev.isMapped(addr, 3)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read16(a);
    			x |= (uint64_t)dev->bus_read8(a + 2) << 16;
    			x |= (uint64_t)read8(a + 3) << 24;
    			x |= (uint64_t)read32(a + 4) << 32;
    			return x;
    		}
    		else if (dev.isMapped(addr, 2)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read16(a);
    			x |= (uint64_t)read16(addr + 2) << 16;
    			x |= (uint64_t)read32(addr + 4) << 32;
    			return x;
    		}
    		else if (dev.isMapped(addr)) {
    			uint64_t x = 0;
    			x |= (uint64_t)dev->bus_read8(a);
    			x |= (uint64_t)read8(addr + 1) << 8;
    			x |= (uint64_t)read16(addr + 2) << 16;
    			x |= (uint64_t)read32(addr + 4) << 32;
			    return x;
    		}
    	}
    	return 0;
    }
}

#endif /* UVMDP_BUS_HPP */
