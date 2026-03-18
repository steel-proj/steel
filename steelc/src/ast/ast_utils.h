#pragma once

template<typename T>
inline T* ast_cast(const ast_node* node) {
	return dynamic_cast<T*>(node);
}
template<typename T>
inline T& ast_cast(const ast_node& node) {
	return dynamic_cast<T&>(node);
}
template<typename T>
inline std::unique_ptr<T> ast_cast(std::unique_ptr<ast_node> node) {
	return std::unique_ptr<T>(dynamic_cast<T*>(node.release()));
}
