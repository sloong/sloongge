#pragma once

namespace SoaringLoong
{
	namespace Universal
	{
		typedef LPTHREAD_START_ROUTINE LPCALLBACKFUNC;
		struct ThreadParam
		{
			LPCALLBACKFUNC	pJob;
			LPVOID			pParam;
		};
		class CThreadPool
		{
		public:
			CThreadPool();
			virtual ~CThreadPool();

			virtual void Initialize( int nThreadNum );

			virtual void Start();
			virtual void End();
			// Add a task to job list, and return the task index.
			virtual int AddTask( LPCALLBACKFUNC pJob, LPVOID pParam );
			// remove a task from job list.
			virtual void RemoveTask(int index);
			virtual int GetTaskTotal();
			virtual HANDLE GetJobListMutex();
		protected:
			vector<HANDLE>* m_pThreadList;
			static list<ThreadParam*>*	m_pJobList;
			static DWORD WINAPI ThreadWorkLoop(LPVOID lpParam);
			static HANDLE g_pMutex;
		};
	}
}