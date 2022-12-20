#include <type_traits>
#include <concepts>

template<typename T>
concept copy_assignable = std::is_copy_assignable_v<T>;

template<typename T>
concept move_assignable = std::is_move_assignable_v<T>;

template<typename T, typename U>
concept copy_assignable_from = std::assignable_from<typename std::add_lvalue_reference<T>::type, typename std::add_lvalue_reference <typename std::add_const<U>::type>::type>;

template<typename T, typename U>
concept move_assignable_from = std::assignable_from<typename std::add_lvalue_reference<T>::type, typename std::add_rvalue_reference<U>::type>;

template<typename T, typename U>
concept copy_constructible_from = std::constructible_from<T, typename std::add_lvalue_reference<typename std::add_const<U>::type>::type>;

template<typename T, typename U>
concept move_constructible_from = std::constructible_from<T, typename std::add_rvalue_reference<U>::type>;

struct null_opt{
	null_opt() = default;
};

template <typename T>
union LocalStorage {
	null_opt garbage;
	T object;

	constexpr LocalStorage() noexcept : garbage() {}
	constexpr LocalStorage(const T& other) noexcept(noexcept(T(other))) {
		std::construct_at(&(this->object), other);
		//new(&(this->object)) T(other);
	}
	constexpr LocalStorage(T&& other) noexcept(noexcept(T(std::move(other)))){
		std::construct_at(&(this->object), std::move(other));
		//new(&(this->object)) T(std::move(other));
	}

	constexpr ~LocalStorage() {}
};



template <typename T>
class optional {
private:
	LocalStorage<T> storage;
	bool state;

public:
	constexpr optional() noexcept : storage(), state(false) {}
	
	constexpr optional(const T& other) noexcept(noexcept(T(other))) requires copy_constructible_from<T, T> : storage(other), state(true) {}

	constexpr optional(T&& other) noexcept(noexcept(T(std::move(other)))) requires move_constructible_from<T, T> : storage(std::move(other)), state(true) {}

	constexpr void reset() noexcept {
		if (!state)
			return;
		storage.object.~T();
		state = false;
	}

	constexpr optional& operator=(const T& other) requires copy_assignable<T> && copy_constructible_from<T, T> {
		if (state)
			storage.object = other;
		else
			std::construct_at(&(storage.object), other);
			//new(&(storage.object)) T(other);
		state = true;
		return *this;
	}

	constexpr optional& operator=(T&& other) requires move_assignable<T> && move_constructible_from<T, T> {
		if (state)
			storage.object = std::move(other);
		else
			std::construct_at(&(storage.object), std::move(other));
			//new(&(storage.object)) T(std::move(other));
		state = true;
		return *this;
	}

	constexpr T& operator*()& {
		if (!state)
			throw std::runtime_error::runtime_error("Optional is Empty!\n");
		return storage.object;
	}

	constexpr const T& operator*() const& {
		if (!state)
			throw std::runtime_error::runtime_error("Optional is Empty!\n");
		return storage.object;
	}

	constexpr T&& operator*()&& {
		if (!state)
			throw std::runtime_error::runtime_error("Optional is Empty!\n");
		return std::move(storage.object);
	}

	constexpr T* operator->() {
		if (!state)
			throw std::runtime_error::runtime_error("Optional is Empty!\n");
		return &(storage.object);
	}

	constexpr const T* operator->() const {
		if (!state)
			throw std::runtime_error::runtime_error("Optional is Empty!\n");
		return &(storage.object);
	}


	constexpr optional(const optional<T>& other) requires copy_constructible_from<T, T> : storage(), state(other.state) {
		if (other.state)
			std::construct_at(&storage.object, other.storage.object);
	}

	constexpr optional(optional<T>&& other) requires copy_constructible_from<T, T> : storage(), state(other.state) {
		if(other.state)
			std::construct_at(&storage.object, std::move(other.storage.object));
	}

	template<typename U>
	constexpr optional(const optional<U>& other) requires copy_constructible_from<T, U> : storage(), state(other.state) {
		if (other.state)
			std::construct_at(&storage.object, other.storage.object);
	}

	template<typename U>
	constexpr optional(optional<U>&& other) requires std::constructible_from<T, U> : storage(), state(other.state) {
		if (other.state)
			std::construct_at(&storage.object, std::move(other.storage.object));
	}

	template<typename U>
	constexpr optional& operator=(const optional<U>& other)
		noexcept(std::is_nothrow_constructible_v<T, typename std::add_lvalue_reference<typename std::add_const<U>::type>::type>)
		requires copy_assignable_from<T, U> && copy_constructible_from<T, U> {
		if (!other.state) {
			reset();
			return *this;
		}
		if (!state)
			std::construct_at(&(storage.object), other.storage.object);
		else
			storage.object = other.storage.object;
		state = true;
		return *this;
	}

	constexpr optional& operator=(const optional<T>& other)
		noexcept(std::is_nothrow_constructible_v<T, typename std::add_lvalue_reference<typename std::add_const<T>::type>::type>)
		requires copy_assignable_from<T, T> && copy_constructible_from<T, T> { 
		return operator=<T>(other); 
	}

	template<typename U>
	constexpr optional& operator=(optional<U>&& other)
		noexcept(noexcept(std::is_nothrow_constructible_v<T, typename std::add_rvalue_reference<U>::type>))
		requires move_assignable_from<T, U> && move_constructible_from<T, U> {
		if (!other.state) {
			reset();
			return *this;
		}
		if (!state)
			std::construct_at(&(storage.object), std::move(other.storage.object));
		else
			storage.object = std::move(other.storage.object);
		state = true;
		other.reset();
		return *this;
	}

	constexpr optional& operator=(optional<T>&& other)
		noexcept(noexcept(std::is_nothrow_constructible_v<T, typename std::add_rvalue_reference<T>::type>))
		requires move_assignable_from<T, T>&& move_constructible_from<T, T> {
		return operator=<T>(std::move(other));
	}

	template<typename... Args>
	constexpr void emplace(Args&&... args)
		noexcept(noexcept(T(std::forward(args))))
		requires std::constructible_from<T, Args...> {
		if (state)
			storage.object.~T();
		std::construct_at(&(storage.object), std::forward<Args>(args)...);
		state = true;
	}

	constexpr T& value_or(T& x) const noexcept {
		if (state)
			return storage.object;
		return x;
	}

	explicit constexpr operator bool() const noexcept {
		return state;
	}

	constexpr ~optional() noexcept {
		if (state) {
			storage.object.~T();
		}
	}
};

template<typename T, typename... Args>
optional<T> make_optional(Args&&... args)
	noexcept(noexcept(emplace(args...)))
	requires std::constructible_from<T, Args...> {
	optional<T> rval = optional<T>();
	rval.emplace(std::forward<Args>(args)...);
	return rval;
}
