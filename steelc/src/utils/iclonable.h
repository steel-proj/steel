#pragma once

#include <memory>

#define ENABLE_CLONE(T, CloneT) std::unique_ptr<CloneT> clone() const override { \
	auto cloned = std::make_unique<T>(); \
	clone_into(*cloned); \
	return cloned; \
}
#define DISABLE_CLONE(T, CloneT) std::unique_ptr<CloneT> clone() const override { \
	s_fatal("Illegal call to " #T "::clone()!"); \
}

template<typename T>
class iclonable {
public:
	virtual std::unique_ptr<T> clone() const = 0;

protected:
	virtual void clone_into(T& target) const = 0;
};