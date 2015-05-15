#pragma once

namespace SoaringLoong
{
	namespace Universal
	{
		typedef enum _ThreadStatus
		{
			Run,
			Stop,
		}THREAD_STATUS;
		class CThread
		{
		public:
			CThread();
			virtual ~CThread();

			virtual void Initialize();
			virtual void Start();
			virtual void Stop();
			virtual void GetStatus();
			virtual void SetStatus();
			virtual void SetJob();
			virtual void GetJob();


		protected:
			THREAD_STATUS m_emStatus;

		};
	}
}