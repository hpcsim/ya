#ifndef DATAEXCEPTION_H
#define DATAEXCEPTION_H


#include <exception>
#include <string>


namespace yandex {
namespace data {


class DataException: public std::exception
{
	
	public:
		DataException(const std::string &);
		~DataException() throw();
	
	
	private:
		std::string error_;

	public:
		const char * what() const throw();
	
};


} //data
} //yandex

#endif
