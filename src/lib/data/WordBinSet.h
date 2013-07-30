#ifndef WORDBINSET_H
#define WORDBINSET_H


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/filesystem.hpp>


namespace yandex {
namespace data {


struct WordBin
{
	WordBin(const std::string& w, size_t s):word(w),size(s){}
	
	std::string word;
	size_t size;

};

// define a multiply indexed set with indices by word and size
typedef boost::multi_index::multi_index_container<
	WordBin,
	boost::multi_index::indexed_by<
		// sort by less<std::string> on word
		boost::multi_index::hashed_unique< boost::multi_index::member < WordBin, std::string, &WordBin::word > >,
		// sort by less<size_t> on size
		boost::multi_index::ordered_non_unique< boost::multi_index::member < WordBin, size_t, &WordBin::size > >
		> 
	> wordbin_set_t;
// wordbin_set_t sorted by word (search log(1) or n in worst case)
typedef wordbin_set_t wordbin_set_by_word_t;
// wordbin_set_t sorted by size (search log(n))
typedef wordbin_set_t::nth_index<1>::type wordbin_set_by_size_t;

} //data
} //yandex

#endif
