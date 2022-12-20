#include <iostream>
#include <vector>
#include "Optional.h"
#include <concepts>

struct MoveAndDel {
	int a;
	MoveAndDel() noexcept = default;
	MoveAndDel(const int& other) noexcept: a(other) {
	}
	MoveAndDel(int&& other) noexcept: a(std::move(other)) {
	}
	MoveAndDel(const MoveAndDel& other) noexcept : a(other.a) {
	}
	MoveAndDel(MoveAndDel&& other) noexcept : a(std::move(other.a)) {
	}
	MoveAndDel& operator=(const MoveAndDel& other) {
		a = other.a;
		return *this;
	}
	MoveAndDel& operator=(MoveAndDel&& other) noexcept {
		a = std::move(other.a);
		return *this;
	}

	~MoveAndDel() {
	}


};



int main() {
	
	
}