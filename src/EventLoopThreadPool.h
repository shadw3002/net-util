#pragma once

#include <vector>
#include <memory>
#include "Thread.h"
#include "EventLoopThread.h"

class EventLoopThreadPool
{
public:
	typedef EventLoopThread::CallbackType CallbackType;

	explicit EventLoopThreadPool(int threadCnt);
	~EventLoopThreadPool();

	void SetReadCallback(CallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);

	void PushFd(int fd);
	void Loop();
	void Quit();
	int GetThreadCnt();
	bool Empty();

private:
	void RunInThread(int threadId);

private:
	int m_threadCnt;
	int m_robinIndex;
	std::vector<std::unique_ptr<Thread>> m_threads;
	std::vector<EventLoopThread *> m_eventLoopThreads;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
};

EventLoopThreadPool::EventLoopThreadPool(int threadCnt) : m_threadCnt(threadCnt),
														m_robinIndex(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
	Quit();
}

void EventLoopThreadPool::PushFd(int fd)
{
	m_robinIndex %= m_threadCnt;
	m_eventLoopThreads[m_robinIndex]->PushFd(fd);
	m_robinIndex += 1;
}

int EventLoopThreadPool::GetThreadCnt()
{
	return m_threadCnt;
}

bool EventLoopThreadPool::Empty()
{
	return 0 == m_threadCnt;
}

void EventLoopThreadPool::RunInThread(int threadId)
{
	EventLoopThread eventLoopThread;
	m_eventLoopThreads[threadId] = static_cast<EventLoopThread *>(&eventLoopThread);
	eventLoopThread.SetReadCallback(m_readCallback);
	eventLoopThread.SetWriteCallback(m_writeCallback);
	eventLoopThread.SetErrorCallback(m_errorCallback);
	eventLoopThread.SetThreadId(threadId);
	eventLoopThread.Loop();
}

void EventLoopThreadPool::Loop()
{
	m_threads.reserve(m_threadCnt);
	m_eventLoopThreads.resize(m_threadCnt);
	for (int i = 0; i < m_threadCnt; ++i)
	{
		m_threads.emplace_back(new Thread(std::bind(&EventLoopThreadPool::RunInThread, this, i)));
		m_threads[i]->Start();
	}
}

void EventLoopThreadPool::Quit()
{
	for (int i = 0; i < m_threadCnt; ++i)
	{
		m_eventLoopThreads[i]->Quit();
		m_threads[i]->Join();
	}

	std::vector<std::unique_ptr<Thread>> temp;
	m_threads.swap(temp);
}

void EventLoopThreadPool::SetReadCallback(CallbackType readCallback)
{
	m_readCallback = readCallback;
}

void EventLoopThreadPool::SetWriteCallback(CallbackType writeCallback)
{
	m_writeCallback = writeCallback;
}

void EventLoopThreadPool::SetErrorCallback(CallbackType errorCallback)
{
	m_errorCallback = errorCallback;
}