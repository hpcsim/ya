#include "ThreadController.h"
#include <iostream>
#include <boost/filesystem/fstream.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "DataException.h"


using namespace yandex::data;


void ThreadController::InterruptAllExceptId(size_t id)
{
	for(size_t thread_id = 0; thread_id < thread_num_; thread_id++)
		if(thread_id!=id)
			threads_[thread_id]->interrupt();
}


bool ThreadController::getFileToParse(boost::filesystem::path & path)
{
	bool parse_file = false;
					
	job_spinlock_->lock();
							
	filejob_set_t::reverse_iterator job_set_it = job_set_.rbegin();
	if(job_set_it != job_set_.rend())
	{

		path = boost::filesystem::path(job_set_it->path_);
		parse_file = true;
		filejob_set_t::iterator job_set_it_last = job_set_.end();
		job_set_it_last--;
		job_set_.erase(job_set_it_last);
	}
					
	job_spinlock_->unlock();
					
	return parse_file;

}

void ThreadController::threadFunction(size_t id, ThreadController * thread_controller)
{
	try
	{
		while(thread_controller->threads_command_[id] != EXIT)
		{
			// if some slave thread throws exception it gives other threads to finish parsing file 
			boost::this_thread::interruption_point();

			switch(thread_controller->threads_command_[id])
			{
				case WORK:
					{
						boost::filesystem::path path_p;
						if(thread_controller->getFileToParse(path_p))
							thread_controller->threadParseFile(path_p);

					}
				break;
				case FINISH:
					{
						boost::filesystem::path path_p;
						if(thread_controller->getFileToParse(path_p))
							thread_controller->threadParseFile(path_p);
						else
							thread_controller->threads_command_[id] = EXIT;

					}
					break;
				case EXIT:
					break;
			}

		}

	} 
	catch(boost::thread_interrupted & e)
	{
		return;
	}
	catch(std::exception & e)
	{
		if(thread_controller->error_spinlock_->try_lock())
		{
			std::ostringstream exception_help;
			exception_help << "Exception in thread [" << id << "]: "
				<< e.what();
			thread_controller->InterruptAllExceptId(id);
			thread_controller->error_ = boost::copy_exception(DataException(exception_help.str()));
		};
	}
};


void ThreadController::threadParseFile(const boost::filesystem::path & path)
{
	
	cout_spinlock_->lock();
	std::cout << "processing...:" << path << std::endl;
	cout_spinlock_->unlock();

	std::string file_content;
	if(boost::filesystem::file_size(path) > file_content.max_size())
		throw DataException("STRING: too big filesize for reading");

	boost::filesystem::ifstream file;

	file.open(path.string().c_str(),std::ios::in);
	
	if (!file) 
		throw DataException("STRING: could not read file");
	
	file_content = static_cast< const std::stringstream & >(std::stringstream() << file.rdbuf()).str();
		
	boost::regex word_regex("[A-Za-z0-9]+");
	
	for( boost::sregex_iterator it_regex(file_content.begin(), file_content.end(), word_regex), it_end_regex;
		it_regex!=it_end_regex;
		it_regex++)
	{
		std::string key = boost::to_upper_copy(it_regex->str());
  		
		// get the set by index word
		histogram_spinlock_->lock();
		wordbin_set_by_word_t & histogram_by_word = *word_histogram_;
		wordbin_set_by_word_t::iterator it_histogram = histogram_by_word.find(key);
		if(it_histogram == histogram_by_word.end())
		{
			histogram_by_word.insert(WordBin(key, 1));
		}
		else
		{
			WordBin wordbin = *it_histogram;
			wordbin.size++;
			histogram_by_word.replace(it_histogram, wordbin);
		}
		
		histogram_spinlock_->unlock();
		
	};

};
