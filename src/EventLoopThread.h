#pragma once

#include <vector>
#include <memory>
#include "Thread.h"


#include <vector>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Mutex.h"
#include "Channel.h"
#include "EventLoop.h"

class EventLoopThread
{
public:
	typedef Channel* ChannelPtr;
	typedef Channel::Callback CallbackType;

	EventLoopThread();
	~EventLoopThread();

	EventLoopThread(const EventLoopThread &) = delete;
	EventLoopThread & operator=(const EventLoopThread &) = delete;

	void SetReadCallback(CallbackType readCallback);
	void SetWriteCallback(CallbackType writeCallback);
	void SetErrorCallback(CallbackType errorCallback);
	void SetThreadId(int threadId);

	void PushFd(int fd);
	void Loop();
	void Quit();

private:
	void OnReadHandler(Channel* ptChannel);

private:
	int m_pipeFd[2];	// to arouse the sub-thread to add socket fd in m_vecQue to m_eventloop with EPOLLIN event
	ChannelPtr m_listenChannel;
	MutexLock m_mutex;		// protected the m_vecQue
	std::vector<int> m_vecQue;
	EventLoop m_eventLoop;
	CallbackType m_readCallback;
	CallbackType m_writeCallback;
	CallbackType m_errorCallback;
	int m_threadId;
};

EventLoopThread::EventLoopThread()
{
	pipe2(m_pipeFd, O_CLOEXEC | O_NONBLOCK);
	m_listenChannel = new Channel(&m_eventLoop, m_pipeFd[0]);
	m_vecQue.reserve(32);
}

EventLoopThread::~EventLoopThread()
{
	Quit();
	close(m_pipeFd[0]);
	close(m_pipeFd[1]);
}

void EventLoopThread::OnReadHandler(Channel* ptChannel)
{
	char buff[1024];

	MutexLockGuard lock(m_mutex);
	read(m_pipeFd[0], buff, sizeof(buff));
	size_t queSize = m_vecQue.size();
	for (size_t i = 0; i < queSize; ++i)
	{
		ChannelPtr ptChannel = new Channel(&m_eventLoop, m_vecQue[i]);
		ptChannel->set_read_callback(m_readCallback);
		ptChannel->set_write_callback(m_writeCallback);
		ptChannel->set_error_callback(m_errorCallback);
		m_eventLoop.add_channel(ptChannel);
		// m_eventLoop.AddLastActivedTime(ptChannel, m_eventLoop.GetLastActiveTime());
	}
	m_vecQue.clear();
}

void EventLoopThread::Loop()
{
	using namespace std::placeholders;
	m_listenChannel->set_read_callback(std::bind(&EventLoopThread::OnReadHandler, this, m_listenChannel));
	m_eventLoop.add_channel(m_listenChannel);
	m_eventLoop.loop();
}

void EventLoopThread::PushFd(int fd)
{
	MutexLockGuard lock(m_mutex);
	m_vecQue.push_back(fd);
	write(m_pipeFd[1], " ", 1);
}

void EventLoopThread::SetReadCallback(CallbackType readCallback)
{
	m_readCallback = readCallback;
}

void EventLoopThread::SetWriteCallback(CallbackType writeCallback)
{
	m_writeCallback = writeCallback;
}

void EventLoopThread::SetErrorCallback(CallbackType errorCallback)
{
	m_errorCallback = errorCallback;
}

void EventLoopThread::Quit()
{
	m_eventLoop.quit();
}

void EventLoopThread::SetThreadId(int threadId)
{
	m_threadId = threadId;
}