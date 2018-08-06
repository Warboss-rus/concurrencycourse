#pragma once
#include <mutex>
#include <memory>
#include <optional>

template <typename T>
class optimized_thread_safe_queue
{
private:
	struct node
	{
		std::optional<T> data;
		std::unique_ptr<node> next;
	};

	std::mutex head_mutex;
	std::unique_ptr<node> head;
	std::mutex tail_mutex;
	node* tail;
	std::condition_variable data_cond;

	node* get_tail()
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex);
		return tail;
	}

	std::unique_ptr<node> pop_head()
	{
		std::unique_ptr<node> old_head = std::move(head);
		head = std::move(old_head->next);
		return old_head;
	}

	std::unique_ptr<node> try_pop_head()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		return pop_head();
	}

	std::unique_ptr<node> try_pop_head(T& value)
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		if (head.get() == get_tail())
		{
			return std::unique_ptr<node>();
		}
		value = std::move(*head->data);
		return pop_head();
	}

	std::unique_lock<std::mutex> wait_for_data()
	{
		std::unique_lock<std::mutex> head_lock(head_mutex);
		data_cond.wait(head_lock, [&] { return head.get() != get_tail(); });
		return std::move(head_lock);
	}

	std::unique_ptr<node> wait_pop_head()
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		return pop_head();
	}

	std::unique_ptr<node> wait_pop_head(T& value)
	{
		std::unique_lock<std::mutex> head_lock(wait_for_data());
		value = std::move(*head->data);
		return pop_head();
	}

public:
	optimized_thread_safe_queue()
		: head(new node)
		, tail(head.get())
	{
	}

	~optimized_thread_safe_queue()
	{
		std::vector<node*> nodes;
		auto current_node = head.get();
		while (current_node)
		{
			nodes.emplace_back(current_node);
			current_node = current_node->next.get();
		}
		for (auto it = nodes.rbegin(); it != nodes.rend(); ++it)
		{
			(*it)->next.reset();
		}
	}

	std::optional<T> try_pop()
	{
		std::unique_ptr<node> old_head = try_pop_head();
		return old_head ? std::move(old_head->data) : std::optional<T>();
	}

	bool try_pop(T& value)
	{
		std::unique_ptr<node> const old_head = try_pop_head(value);
		return old_head;
	}

	std::optional<T> wait_and_pop()
	{
		std::unique_ptr<node> const old_head = wait_pop_head();
		return std::move(old_head->data);
	}

	void wait_and_pop(T& value)
	{
		std::unique_ptr<node> const old_head = wait_pop_value(value);
	}

	void push(T new_value)
	{
		std::unique_ptr<node> p(new node);
		{
			std::lock_guard<std::mutex> tail_lock(tail_mutex);
			tail->data = std::move(new_value);
			node* const new_tail = p.get();
			tail->next = std::move(p);
			tail = new_tail;
		}
		data_cond.notify_one();
	}

	void empty()
	{
		std::lock_guard<std::mutex> head_lock(head_mutex);
		return (head.get(0 == get_tail()));
	}
};