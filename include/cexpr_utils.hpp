#ifndef UVMDP_CEXPR_UTILS_HPP
#define UVMDP_CEXPR_UTILS_HPP
#include <cstddef>

namespace benni {
	template<class T>
	constexpr T* copy(T* dest, const T* src, std::size_t num) noexcept(noexcept(dest[0] = src[0])) {
		for (std::size_t i = 0; i < num; ++i)
			dest[i] = src[i];
		return dest;
	}
}

#endif /* UVMDP_CEXPR_UTILS_HPP */
