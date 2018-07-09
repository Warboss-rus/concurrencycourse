#pragma once

template <typename Thread>
class JoinableThread
{
public:
	JoinableThread()
	{
	}

	template <typename T>
	JoinableThread(T && f)
		:m_thread(std::forward<T>(f))
	{
	}

	JoinableThread(JoinableThread && x)
		:m_thread(std::move(x.m_thread))
	{
	}

	JoinableThread & operator=(JoinableThread && x)
	{
		return *this = std::move(x.m_thread);
	}

	JoinableThread & operator=(Thread && thread)
	{
		if (&m_thread != &thread)
		{
			JoinIfJoinable();
			m_thread = std::move(thread);
		}
		return *this;
	}

	~JoinableThread()
	{
		JoinIfJoinable();
	}

	bool joinable()const
	{
		return m_thread.joinable();
	}

	void join()
	{
		m_thread.join();
	}

	Thread & get()
	{
		return m_thread;
	}

private:
	void JoinIfJoinable()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}
		
	Thread m_thread;
};
