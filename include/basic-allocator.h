//
// Created by liu on 01.05.2021.
//

#ifndef BASIC_SERVICES_BASIC_ALLOCATOR_H
#define BASIC_SERVICES_BASIC_ALLOCATOR_H

#include "basic-services_export.h"

namespace basic {

template<typename T>
class ObjectTraits {
public:
	//! Constructor
	ObjectTraits() = default;

	//! Destructor
	~ObjectTraits() = default;

	//! Copy constructor
	ObjectTraits(ObjectTraits  const&) = default;

	template <typename U>
	explicit ObjectTraits(ObjectTraits<U> const&) {}

	//! Convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef ObjectTraits<U> other;
	};

	// Get address
	inline T* address(T& r) { return &r; }
	inline T const* address(T const& r) { return &r; }

	// Construction/destruction
	inline static void construct(T* p, const T& t) { new (p) T(t); }
	inline static void destroy(T* p) { p->~T(); }
};

template<typename T>
class AllocPolicy {
public:
	using value_type        = T;
	using pointer           = value_type *;
	using const_pointer     = const value_type *;
	using reference         = value_type&;
	using const_reference   = const value_type&;
	using size_type         = std::size_t ;
	using difference_type   = std::ptrdiff_t;

	template<typename U>
	struct rebind {
		typedef AllocPolicy<U> other;
	};

	//! Constructor
	AllocPolicy() = default;

	//! Destructor
	~AllocPolicy() = default;

	//! Copy constructor
	AllocPolicy(AllocPolicy const&) = default;

	template <typename U>
	inline explicit AllocPolicy(AllocPolicy<U> const&) {}

	//! Allocate memory
	inline pointer allocate(size_type cnt,
	                        typename std::allocator<void>::const_pointer = 0) {
		return reinterpret_cast<pointer>(::operator new(cnt * sizeof (T)));
	}

	//! Deallocate memory
	inline void deallocate(pointer p, size_type) { ::operator delete(p); }

	inline size_type max_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}
};

// determines if memory from another allocator can be deallocated from this one
template<typename T, typename T2>
inline bool operator==(AllocPolicy<T> const&,
                       AllocPolicy<T2> const&) {
	return true;
}
template<typename T, typename OtherAllocator>
inline bool operator==(AllocPolicy<T> const&, OtherAllocator const&) {
	return false;
}

//! A simple allocator implementation
template<typename T,
		typename Policy = basic::AllocPolicy<T>,
		typename Traits = basic::ObjectTraits<T> >
class Allocator : public Policy, public Traits {
private:
	using AllocationPolicy = Policy ;
	using ObjectTraits = Traits;

public:
	using size_type         = typename AllocationPolicy::size_type;
	using difference_type   = typename AllocationPolicy::difference_type ;
	using pointer           = typename AllocationPolicy::pointer;
	using const_pointer     = typename AllocationPolicy::const_pointer;
	using reference         = typename AllocationPolicy::reference;
	using const_reference   = typename AllocationPolicy::const_reference;
	using value_type        = typename AllocationPolicy::value_type;

	template <typename U>
	struct rebind {
		typedef Allocator<U> other;
	};

	//! Constructor
	Allocator() = default;

	//! Destructor
	~Allocator() = default;

	Allocator(Allocator const& rhs):Traits(rhs), Policy(rhs) {}

	template <typename U>
	explicit Allocator(Allocator<U> const&) {}

	template <typename U, typename P, typename T2>
	explicit Allocator(Allocator<U, P, T2> const& rhs):Traits(rhs), Policy(rhs) {}

	//! Allocate memory
	inline pointer allocate(size_type cnt,
	                        typename std::allocator<void>::const_pointer cp = nullptr) {
		return AllocationPolicy::allocate(cnt, cp);
	}

	//! Deallocate memory
	inline void deallocate(pointer p, size_type cnt) {
		AllocationPolicy::deallocate(p, cnt);
	}
};

template<typename T, typename P, typename Tr>
inline bool operator==(Allocator<T, P, Tr> const& lhs,
                       Allocator<T, P, Tr> const& rhs) {
	return operator==(static_cast<P&>(lhs), static_cast<P&>(rhs));
}

template<typename T, typename P, typename Tr, typename T2,
		typename P2, typename Tr2>
inline bool operator==(Allocator<T, P, Tr> const& lhs,
                       Allocator<T2, P2, Tr2> const& rhs) {
	return operator==(static_cast<P&>(lhs), static_cast<P2&>(rhs));
}

template<typename T, typename P, typename Tr, typename OtherAllocator>
inline bool operator==(Allocator<T, P, Tr> const& lhs,
                       OtherAllocator const& rhs) {
	return operator==(static_cast<P&>(lhs), rhs);
}

template<typename T, typename P, typename Tr>
inline bool operator!=(Allocator<T, P, Tr> const& lhs,
                       Allocator<T, P, Tr> const& rhs) {
	return !operator==(lhs, rhs);
}

template<typename T, typename P, typename Tr, typename T2,
		typename P2, typename Tr2>
inline bool operator!=(Allocator<T, P, Tr> const& lhs,
                       Allocator<T2, P2, Tr2> const& rhs) {
	return !operator==(lhs, rhs);
}

template<typename T, typename P, typename Tr, typename OtherAllocator>
inline bool operator!=(Allocator<T, P, Tr> const& lhs,
                       OtherAllocator const& rhs) {
	return !operator==(lhs, rhs);
}

} // namespace basic

#endif //BASIC_SERVICES_BASIC_ALLOCATOR_H
