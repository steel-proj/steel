#include "math_utils.h"

#include <cstdint>

int math_utils::count_digits(size_t n) {
	if (n == 0) {
		return 1;
	}
	
	int32_t count = 0;
	while (n > 0) {
		n /= 10;
		count++;
	}
	
	return count;
}