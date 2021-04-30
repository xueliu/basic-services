//
// Created by liu on 04.01.2021.
//

#ifndef BASIC_SERVICES_THREAD_SAFE_STACK_H
#define BASIC_SERVICES_THREAD_SAFE_STACK_H

namespace basic {

template<typename T>
class ThreadSafeStack {
private:
	std::stack<T> m_stack;
	mutable std::mutex m_mtx;

public:
	ThreadSafeStack() = default;

	ThreadSafeStack(const ThreadSafeStack & rhs) {
		std::lock_guard<std::mutex> lk(rhs.m_mtx);
		m_stack = rhs.m_stack;
	}

	ThreadSafeStack& operator = (const ThreadSafeStack& rhs) = delete;

	void Push(T v) {
		std::lock_guard<std::mutex>lk(m_mtx);

		m_stack.push(std::move(v));
	}

	std::shared_ptr<T> Pop() {
		std::lock_guard<std::mutex>lk(m_mtx);
		if(m_stack.empty())
			return  std::shared_ptr<T>();
		std::shared_ptr<T> const ret (std::make_shared<T>(std::move(m_stack.top())));
		m_stack.pop();

		return ret;
	}

	bool Pop(T& v) {
		std::lock_guard<std::mutex>lk(m_mtx);
		if(m_stack.empty())
			return false;

		v = std::move( m_stack.top() );
		m_stack.pop();
	}

	bool Empty() const {
		std::lock_guard<std::mutex> lk(m_mtx);
		return m_stack.empty();
	}
};

} // namespace basic

#endif //BASIC_SERVICES_THREAD_SAFE_STACK_H
