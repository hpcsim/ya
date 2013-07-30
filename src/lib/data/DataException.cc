#include"DataException.h"


using namespace yandex::data;


DataException::DataException(const std::string & error) :
		error_(error)
{
}

DataException::~DataException() throw()
{
}

const char * DataException::what() const throw()
{
	return error_.c_str();
}
