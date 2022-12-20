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
	MoveAndDel val = 5;
	optional<MoveAndDel> test(val);
	test = MoveAndDel(5);
	std::cout << (*test).a << '\n';
	optional<int> new_test(100000);
	optional<int> another;
	another = std::move(new_test);
	constexpr optional<int> n;
	constexpr optional<int> duuh = optional<int>(5);
	std::cout << bool(another) << '\n';
	std::cout << bool(n) << '\n';
	
}