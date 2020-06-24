#ifndef UVMDP_MEMORY_HPP
#define UVMDP_MEMORY_HPP
#include <cstdio>
#include "cexpr_utils.hpp"
#include "device.hpp"

namespace benni {
	class ROM : public BusDevice {
	protected:
		uint8_t* memory;
	public:
		constexpr explicit ROM(address_t size) : BusDevice(size), memory(new uint8_t[size]) {}
		constexpr ROM(const ROM& rom) : BusDevice(rom.bus_size),
			memory(copy(new uint8_t[rom.bus_size], rom.memory, bus_size)) {}
		constexpr ROM(ROM&& rom) noexcept : BusDevice(rom.bus_size), memory(rom.memory) { rom.memory = nullptr; }
		constexpr ~ROM() noexcept override { delete[] memory; memory = nullptr; }

		inline std::size_t load_file(const char* filename) noexcept;

		constexpr uint8_t bus_read8(address_t a) noexcept override { return memory[a]; }
	};

	class RAM : public ROM {
	public:
		constexpr explicit RAM(address_t size) : ROM(size) {}
		constexpr RAM(const RAM&) = default;
		constexpr RAM(RAM&&) = default;

		constexpr void bus_write8(address_t a, uint8_t b) noexcept override { memory[a] = b; }
	};

	inline std::size_t ROM::load_file(const char* filename) noexcept {
		FILE* file = std::fopen(filename, "rb");
		if (!file) return 0;
		std::size_t n = std::fread(memory, 1, bus_size, file);
		std::fclose(file);
		return n;
	}
}

#endif /* UVMDP_MEMORY_HPP */
