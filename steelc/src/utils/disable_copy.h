#pragma once

#define DISABLE_COPY(T) \
	T(const T&) = delete; \
	T& operator=(const T&) = delete;