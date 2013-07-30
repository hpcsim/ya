#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <boost/filesystem.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <boost/thread/thread.hpp>
#include <boost/exception_ptr.hpp>
#include "WordBinSet.h"

namespace yandex {
namespace data {


class ThreadController
{
	enum thread_command_t {EXIT, WORK, FINISH};

	// class for task
	struct FileJob
	{
		FileJob(uintmax_t size, boost::filesystem::path path) :
				size_(size),
				path_(path)
		{
		}

		uintmax_t size_;
		boost::filesystem::path path_;
	};
	
	// container for job queue sorted by file size (in master/slave architecture haviest tasks should be solved first)
	typedef boost::multi_index::multi_index_container<
		FileJob,
		boost::multi_index::indexed_by<
			// sort by less<size_t> on size
			boost::multi_index::ordered_non_unique< boost::multi_index::member < FileJob, uintmax_t, &FileJob::size_ > >
			> 
		> filejob_set_t;
	

	public:
		ThreadController(size_t thread_num, wordbin_set_t * word_histogram) :
				word_histogram_(word_histogram),
				job_spinlock_(new boost::detail::spinlock()),
				histogram_spinlock_(new boost::detail::spinlock()),
				error_spinlock_(new boost::detail::spinlock()),
				cout_spinlock_(new boost::detail::spinlock()),
				thread_num_(thread_num)
		{
		}
		
		~ThreadController()
		{
			delete job_spinlock_;
			delete histogram_spinlock_;
			delete error_spinlock_;
			delete cout_spinlock_;
			
			for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
			{
				delete threads_[thread_id];
			}
		}

	private:
		// deny access to default copy constructor
		ThreadController(const ThreadController & copy);
		// deny access to default assignment operator
		ThreadController & operator=(const ThreadController & rhs);

	private:
		// word histogram sorted by bin size
		wordbin_set_t * word_histogram_;
		// job queue
		filejob_set_t job_set_;
		// threads commands {EXIT, WORK, FINISH}
		std::vector< thread_command_t > threads_command_;
		// pointer by the exception thrown by slave threads
		boost::exception_ptr error_;
		// lock for job
		boost::detail::spinlock * job_spinlock_;
		// lock for word histogram
		boost::detail::spinlock * histogram_spinlock_;
		// lock for exception throws
		boost::detail::spinlock * error_spinlock_;
		// lock for threads correct std::cout
		boost::detail::spinlock * cout_spinlock_;
		// number of slave threads
		size_t thread_num_;
		// slave threads
		std::vector< boost::thread* > threads_;

	public:
		// create threads
		inline void start()
		{
			for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
				threads_command_.push_back(WORK);

			for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
				threads_.push_back(new boost::thread(threadFunction, thread_id, this));
		
		}

		// task in job queue
		inline void push(const boost::filesystem::path & path)
		{
			if(thread_num_)
			{
				job_spinlock_->lock();
				job_set_.insert(FileJob(boost::filesystem::file_size(path), path));
				job_spinlock_->unlock();
			}
			else
				threadParseFile(path);
		}
	
		// finish tasks in job queue
		inline void finish()
		{
			for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
				threads_command_[thread_id] = FINISH;

			    /**
				* search text files is much faster than processing
				* if you want main thread, instead of just wait, to 
				* join slave threads uncomment further code
				*/
				
			//	threads_command_.push_back(FINISH);
			//	threadFunction(thread_num_, this);

			for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
				threads_[thread_id]->join();
			
			// rethrow slave thread exception
			if(error_)
				boost::rethrow_exception(error_);
		}


	private:
		// static function for threads
		static void threadFunction(size_t id, ThreadController * thread_controller);
		
		// throw interrupts to all slave threads except throwing thread 
		void InterruptAllExceptId(size_t id);
		// pop task from job queue 
		bool getFileToParse(boost::filesystem::path & path);
		// parse file and insert word histogram 
		void threadParseFile(const boost::filesystem::path & path);


};

} //data
} //yandex

#endif
