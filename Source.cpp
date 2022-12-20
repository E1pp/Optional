#include <iostream>
#include <vector>
#include "Optional.h"
#include <concepts>

struct MoveAndDel {
	MoveAndDel() noexcept = default;
	MoveAndDel(const int& other) noexcept {}
	MoveAndDel(int&& other) noexcept {}
	MoveAndDel(const MoveAndDel& other) noexcept {}
	MoveAndDel(MoveAndDel&& other) noexcept  {}
	MoveAndDel& operator=(const MoveAndDel& other) {
		return *this;
	}
	MoveAndDel& operator=(MoveAndDel&& other) noexcept {
		return *this;
	}
	~MoveAndDel() {}
};



int main() {
	
	
}