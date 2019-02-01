#ifndef SORTED_VECTOR_H
#define SORTED_VECTOR_H

#include <vector>
#include <algorithm>

//This is an EXPERIMENTAL sorted vector template class used as a cache-friendly associative container
//It is currently built on std::vector but could ultimately have it's own memory management
//
//Unit test cases can be found in sorted_vector_tests.cpp but this code is under active development and has NOT 
//been thoroughly tested. You're mileage may vary.
//
//STILL TO DO:
//		handle initializer lists?
//		allow duplicates (multi_sorted_vector)?
//		more sadistic unit tests
//		performance test against other associative containers (map, unordered_map, set, et al)
//
template<class T,						//data type
	     class K = int,					//key type
	     class Compare = std::less<T>>	//comparison function
class sorted_vector
{
public:
	explicit sorted_vector(const Compare& cmp = Compare());
	~sorted_vector() {}

	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;
	typedef typename std::vector<T>::reference reference;
	typedef typename std::vector<T>::const_reference const_reference;
	typedef typename std::vector<T*>::size_type size_type;


	sorted_vector(const sorted_vector&) = default;
	sorted_vector(sorted_vector&&) = default;

	sorted_vector& operator= (const sorted_vector&) = default;
	sorted_vector& operator= (sorted_vector&&) = default;


	void reserve(size_type new_cap) { m_vec.reserve(new_cap); }
	size_type size() const { return m_vec.size(); }

	iterator begin() { return m_vec.begin(); }
	iterator end() { return m_vec.end(); }

	const_iterator begin() const { return m_vec.begin(); }
	const_iterator end() const { return m_vec.end(); }

	reference operator[](size_type pos) { return m_vec[pos]; }
	const_reference operator[](size_type pos) const { return m_vec[pos]; }

	iterator insert(const T& value);

	iterator find(const K& value);
	const_iterator find(const K& value) const;

	iterator erase(iterator pos) { return m_vec.erase(pos); }
	iterator erase(const_iterator pos) { return m_vec.erase(pos); }

	iterator erase(iterator first, iterator last) { return m_vec.erase(first, last); }
	iterator erase(const_iterator first, const_iterator last) { return m_vec.erase(first, last); }

	iterator erase(const K& value);


private:
	std::vector<T> m_vec;
	Compare m_cmp;
};

template<class T, class K, class Compare>
sorted_vector<T, K, Compare>::sorted_vector(const Compare& cmp) : 
	m_vec(), 
	m_cmp(cmp)
{}

template<class T, class K, class Compare>
typename std::vector<T>::iterator sorted_vector<T, K, Compare>::insert(const T& value)
{
	auto it = std::lower_bound(m_vec.begin(), m_vec.end(), value, m_cmp);
	if (it == end() || m_cmp(value, *it))	//not there
		return m_vec.insert(it, value);

	return it;
}

template<class T, class K, class Compare>
typename std::vector<T>::iterator sorted_vector<T, K, Compare>::find(const K& value)
{
	auto it = std::lower_bound(m_vec.begin(), m_vec.end(), value, m_cmp);
	if (it == end() || m_cmp(value, *it))	//not there
		return end();

	return it;
}

template<class T, class K, class Compare>
typename std::vector<T>::const_iterator sorted_vector<T, K, Compare>::find(const K& value) const
{
	return find(value);
}

template<class T, class K, class Compare>
typename std::vector<T>::iterator sorted_vector<T, K, Compare>::erase(const K& value)
{
	auto it = std::lower_bound(m_vec.begin(), m_vec.end(), value, m_cmp);
	if (it == end() || m_cmp(value, *it))	//not there
		return end();

	return m_vec.erase(it);
}

#endif //SORTED_VECTOR_H
