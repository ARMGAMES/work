#include <atomic>
#include <list>

template<typename T>
class concurrent_stack
{
	struct concurrent_stack_node;
	typedef std::atomic<concurrent_stack_node*> atomic_list_node_ptr;

	struct concurrent_stack_node
	{
		concurrent_stack_node() : m_prev(nullptr), m_state(not_ready) {}

		enum state
		{
			not_ready,
			ready
		};

		atomic_list_node_ptr m_prev;
		std::atomic<state> m_state;
		T m_t;
	};

public:
	typedef T value_type;

	struct iterator
	{
		iterator(concurrent_stack_node* node = nullptr) : m_node(node) {}
		iterator(const iterator& other) : m_node(other.m_node) {}

		iterator& operator++()
		{
			if (m_node)
			{
				while (m_node->m_state.load(std::memory_order_acquire) == concurrent_stack_node::not_ready) {}
				m_node = m_node->m_prev.load(std::memory_order_acquire);
			}
			return *this;
		}

		iterator operator++(int)
		{
			iterator result(*this);
			++(*this);
			return result;
		}

		bool operator==(const iterator& other) const { return m_node == other.m_node; }
		bool operator!=(const iterator& other) const { return !(*this == other); }

		T* operator->() { return &m_node->m_t; }
		T& operator*() { return m_node->m_t; }

	private:
		concurrent_stack_node * m_node;

		friend class concurrent_stack;
	};

	concurrent_stack()
	{
		m_tail.store(nullptr, std::memory_order_release);
		m_size.store(0, std::memory_order_release);
		m_storage_list_mutex.store(false, std::memory_order_release);
		m_storage_blocks.emplace_back();
		m_current_storage_block = &m_storage_blocks.back();
	}

	iterator push(const T& t)
	{
		concurrent_stack_node* new_node = allocate_node();
		new_node->m_t = t;
		return push_internal(new_node);
	}

	bool pop(T& t)
	{
		concurrent_stack_node* tail = pop_internal();
		if (!tail) return false;
		t = std::move(tail->m_t);
		return true;
	}

	iterator top()
	{
		return rbegin();
	}

	size_t size() const { return m_size.load(std::memory_order_acquire); }
	bool empty() const { return !size(); }
	iterator rbegin() { return iterator(m_tail.load(std::memory_order_acquire)); }
	iterator rend() { return iterator(); }

private:
	concurrent_stack_node * allocate_node()
	{
		while (1)
		{
			memory_block* current_block = m_current_storage_block.load(std::memory_order_acquire);
			size_t idx = current_block->m_next_idx++;
			if (idx < 1000) return &current_block->m_block[idx];
			bool expected = false;
			if (m_storage_list_mutex.compare_exchange_strong(expected, true, std::memory_order_release, std::memory_order_relaxed))
			{
				if (current_block == m_current_storage_block.load(std::memory_order_acquire))
				{
					m_storage_blocks.emplace_back();
					m_current_storage_block.store(&m_storage_blocks.back(), std::memory_order_release);
				}
				m_storage_list_mutex.store(false, std::memory_order_release);
			}
		}
		return nullptr;
	}

	iterator push_internal(concurrent_stack_node* node)
	{
		concurrent_stack_node* oldTail = m_tail.load(std::memory_order_acquire);
		while (1)
		{
			if (oldTail)
			{
				node->m_prev.store(oldTail);
			}
			if (!m_tail.compare_exchange_weak(oldTail, node, std::memory_order_release, std::memory_order_acquire)) continue;
			node->m_state.store(concurrent_stack_node::ready);
			++m_size;
			return iterator(node);
		}
		return iterator();
	}

	concurrent_stack_node* pop_internal()
	{
		concurrent_stack_node* oldTail = m_tail.load(std::memory_order_acquire);
		while (oldTail)
		{
			if (m_tail.compare_exchange_weak(oldTail, oldTail->m_prev, std::memory_order_release, std::memory_order_acquire))
			{
				--m_size;
				break;
			}
		}
		return oldTail;
	}

	struct memory_block
	{
		memory_block() : m_next_idx(0) {}

		concurrent_stack_node m_block[1000];
		std::atomic<size_t> m_next_idx;
	};

	typedef std::list<memory_block> storage_list;

	atomic_list_node_ptr m_tail;
	std::atomic<size_t> m_size;
	std::atomic_bool m_storage_list_mutex;
	storage_list m_storage_blocks;
	std::atomic<memory_block*> m_current_storage_block;
};
