#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/program_options.hpp>
#include "ThreadController.h"
#include "WordBinSet.h"
#include "DataException.h"

namespace
{

int const ERROR_UNHANDLED_EXCEPTION = 1;
int const ERROR_IN_COMMAND_LINE = 2;
int const ERROR_DURING_DIRECTORY_ITERATION = 3;
int const ERROR_IN_DATALIB = 4;

}

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char * argv[]) 
{
	try
	{

		// set default options and their descriptions
		fs::path path = fs::current_path();
		size_t thread_num = boost::thread::hardware_concurrency();
		
		std::ostringstream help_help;
		std::ostringstream indir_help;
		std::ostringstream tnum_help;

		help_help << "Print help messages";
		indir_help << "path to the directory(default: current directory)";
		tnum_help << "number or threads to use (default: " << thread_num << "), if 0 sequential version used ";
		
		// setup cmd options
		po::options_description desc("Options");
		desc.add_options()
			("help,h", help_help.str().c_str())
			("indir,i", po::value<fs::path>(), indir_help.str().c_str())
			("tnum,t", po::value<size_t>(), tnum_help.str().c_str());
		po::variables_map vm;

		// parse options
		try
		{
		    po::store(po::parse_command_line(argc, argv, desc),vm); 
	
			if ( vm.count("help")  )
			{
				std::cout << "This program count words in all \".txt\" files in specified directory and it's subdirectories " << std::endl
					<<"build histogram and shows top 10 results" << std::endl
					<< desc << std::endl;
			    return EXIT_SUCCESS;
			}
			
			if ( vm.count("indir")  )
			{
				path = vm["indir"].as<fs::path>();
			}
			
			if ( vm.count("tnum")  )
			{
				thread_num = vm["tnum"].as<size_t>();
			}
			
			boost::program_options::notify(vm); 
	    }
	    catch(po::error& e)
	    {
	        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return ERROR_IN_COMMAND_LINE;
		}

		std::cout << "THREAD NUMBER:" << thread_num << "\tDIR: " << path << std::endl;
	
		yandex::data::wordbin_set_t word_histogram;
		
		// initiallize object for threads control and parsing files 
		yandex::data::ThreadController thread_controller(thread_num, &word_histogram);


		fs::recursive_directory_iterator it;
		fs::recursive_directory_iterator end;
	
		try
		{
			it = fs::recursive_directory_iterator(path);
		}
		catch(fs::filesystem_error& fex)
		{
			std::cout << fex.what() << std::endl;
			it = fs::recursive_directory_iterator();
		}
		
		// create threads
		thread_controller.start();

		// go throw directories and look for ".txt" files
		while(it != end) 
		{
			// do not go inside link directories	
			if(fs::is_directory(*it) && fs::is_symlink(*it))
				it.no_push();
		
			if(fs::is_regular_file(*it) && fs::extension(*it) == ".txt")
			{
				// push task into the stack, which is executed by free thread  
				thread_controller.push(*it);
			}

			try
			{
				++it;
			}
			catch(std::exception& ex)
			{
				std::cout << ex.what() << std::endl;
				std::cout << "Path ignored:" << *it << std::endl; 
				it.no_push();
				try { ++it; } catch(...) 
				{ 
					std::cerr << "Directory Iteration can not be performed" << std::endl; 
					return ERROR_DURING_DIRECTORY_ITERATION;
				}
			}
		}
		
		// put threads into the finish state
		thread_controller.finish();

		std::cout << "-----------------------------------------" << std::endl;
	
		// get the set by index size (word occure frequency)
		yandex::data::wordbin_set_by_size_t & histogram_by_size = word_histogram.get<1>();
		
		// print first 10
		size_t k = 0;
		for( yandex::data::wordbin_set_by_size_t::reverse_iterator it_histogram = histogram_by_size.rbegin();
			(it_histogram != histogram_by_size.rend()) && (k < 10);
			it_histogram++, k++)
		{
			std::cout << it_histogram->word << " " << it_histogram->size << std::endl;
		}

		std::cout << "-----------------------------------------" << std::endl;
	
	}
	catch(yandex::data::DataException & e)
	{
		std::cerr << e.what() << ", application will now exit" << std::endl;
		return ERROR_IN_DATALIB;
	}
	catch(std::exception& e)
	{
		std::cerr << "Unhandled Exception reached the top of main: "
			<< e.what() << ", application will now exit" << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;
	}
	
	return EXIT_SUCCESS; 

}

