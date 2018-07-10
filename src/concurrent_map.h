#pragma once
#include <map>
#include <mutex>
#include <optional>

template<class Key, class Value>
class concurrent_map
{
public:
	using Pair = std::pair<Key, Value>;
	using FetchHandler = std::function<void(const Pair&)>;

	void Update(const Key& key, const Value& value)
	{
		std::lock_guard lk(m_mutex);
		m_data[key] = value;
	}

	std::optional<Value> Get(const Key& key) const
	{
		std::lock_guard lk(m_mutex);
		if (auto it = m_data.find(key); it != m_data.end())
		{
			return it->second;
		}
		return {};
	}

	void Remove(const Key& key)
	{
		std::lock_guard lk(m_mutex);
		m_data.erase(key);
	}
	
	void Fetch(const Key& key, const FetchHandler& fetchHandler) const
	{
		std::lock_guard lk(m_mutex);
		if (auto it = m_data.find(key); it != m_data.end())
		{
			fetchHandler(*it);
		}
	}

	std::map<Key, Value> Dump() const
	{
		std::lock_guard lk(m_mutex);
		return m_data;
	}

private:
	std::map<Key, Value> m_data;
	mutable std::mutex m_mutex;
};