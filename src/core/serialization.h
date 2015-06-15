#pragma once
//
//
//  Created by ZK on 14-7-4.
//
//

#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include <set>
#include <list>
 
#include "ISerializable.h"

#define  CHECK_ENDIAN 0

class serialize
{
public:
// little endian if true
static bool LE();
static bool ReadEndian(std::istream &istream_);
static void WriteEndian(std::ostream &ostream_);

template<typename T>
static void ZeroMem(T& t)
{
	memset(&t, 0, sizeof (T));
}


static std::istream&  read_internal(std::istream& istream_, char* p, int size);
static std::ostream& write_internal(std::ostream& ostream_, const char* p, int size);

template<typename T>
static std::istream& DeSerialize(std::istream& istream_, T&  t_)
{
	return read_internal(istream_, (char*)&t_, sizeof (t_));
}

template<typename T>
static std::ostream& Serialize(std::ostream& ostream_, T&  t_)
{
	int size = sizeof(t_);
	return write_internal(ostream_, (const char*)&t_, sizeof(t_));
}

 
static std::istream&   DeSerialize(std::istream& istream_, ISerializable* t_);
static std::ostream&	  Serialize(std::ostream& ostream_, ISerializable* t_);
static  std::ostream&  Serialize(std::ostream& ostream_, const std::string& string_);
static  std::ostream&  Serialize(std::ostream& ostream_, std::string& string_);
static  std::istream&  DeSerialize(std::istream& istream_, std::string& string_);
static  std::ostream&  Serialize(std::ostream& ostream_, const char* str);
static  std::istream&  DeSerialize(std::istream& istream_, char* str);


/////////////vector<bool>//////////////////////////////////
//why specialize this ? //http://stackoverflow.com/questions/15809157/why-is-the-size-of-stdvectorbool-16-byte
 
static std::ostream& Serialize(std::ostream& ostream_, std::vector<bool>& container);
static  std::istream& DeSerialize(std::istream& istream_, std::vector<bool>&container);


/////////////vector//////////////////////////////////
template <class T >
static std::ostream& Serialize(std::ostream& ostream_, std::vector<T>& container)
{
	int size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::vector<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.push_back(  t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static std::ostream&  Serialize(std::ostream& ostream_, std::vector<T*>& container)
{

	int size = container.size();
	 
	write_internal(ostream_, (char*)&size, sizeof (int));
	int index = 0;
	for (auto ite = container.begin(); ite != container.end(); ite++)
	{

		if ((*ite) != NULL)
		{
			bool  notNULL = true;
			Serialize(ostream_, notNULL);
 
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool  notNULL = false;
			Serialize(ostream_, notNULL);
		}
		index++;
	}
	return ostream_;
}

template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::vector<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);
	container.resize(size);
	for (int i = 0; i < size; ++i)
	{
		bool  notNULL = false;
		DeSerialize(istream_, notNULL);

		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container[i] = object;
		}
		else
		{
			container[i] = NULL;
		}
	}
	return istream_;
}


////////////////map/////////////////////

template <class K, class V>
static std::ostream&  Serialize(std::ostream& ostream_, std::map<K, V>& container)
{

	int size = container.size();
	Serialize(ostream_, size);

	for (auto p : container)
	{
		Serialize(ostream_, p.first);
		Serialize(ostream_, p.second);


	}
	return ostream_;
}

template <class K, class V>
static  std::istream& DeSerialize(std::istream& istream_, std::map<K, V>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();
	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		K key;
		V value;
		DeSerialize(istream_, key);

		DeSerialize(istream_, value);
		container[key] = value;

	}
	return istream_;
}


template <class K, class V>
static std::ostream&  Serialize(std::ostream& ostream_, std::map<K, V*>& container)
{

	int size = container.size();
	Serialize(ostream_, size);
	int index = 0;
	for (auto p : container)
	{
		Serialize(ostream_, p.first);

		if ((p.second) != NULL)
		{
			bool notNULL = true;
			Serialize(ostream_, notNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(p.second);
			Serialize(ostream_, i);
		}
		else
		{
			bool notNULL = false;
			Serialize(ostream_, notNULL);
		}


		index++;
	}
	return ostream_;
}

template <class K, class V>
static  std::istream& DeSerialize(std::istream& istream_, std::map<K, V*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();
	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		K key;
		DeSerialize(istream_, key);
		bool  notNULL;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			V* object = new V;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container[key] = (V*)object;
		}
		else
		{
			container[key] = NULL;
		}
	}
	return istream_;
}


////////////////set/////////////////////

template <class T >
static std::ostream& Serialize(std::ostream& ostream_, std::set<T>& container)
{
	int size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::set<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.insert(t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static std::ostream&  Serialize(std::ostream& ostream_, std::set<T*>& container)
{

	int size = container.size();
	Serialize(ostream_, size);

	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{
			bool noNULL = true;
			Serialize(ostream_, noNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool noNULL = false;
			Serialize(ostream_, noNULL);
		}

	}
	return ostream_;
}

template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::set<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);
	container.resize(size);
	for (int i = 0; i < size; ++i)
	{
		bool notNULL = false;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container.insert(object);
		}
		else
		{
			container.insert(NULL);
		}
	}
	return istream_;
}



////////////////list/////////////////////

template <class T >
static std::ostream& Serialize(std::ostream& ostream_, std::list<T>& container)
{
	int size = container.size();
	Serialize(ostream_, size);
	for (auto& ite : container)
	{
		Serialize(ostream_, ite);
	}
	return ostream_;
}

template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::list<T>&container)
{
	if (!istream_.good() || istream_.eof())return istream_;

	int size;
	container.clear();
	DeSerialize(istream_, size);
	for (int i = 0; i < size; ++i)
	{
		T t;
		DeSerialize(istream_, t);
		container.push_back(t);
	}
	assert(istream_.good());
	return istream_;
}

template <class T >
static std::ostream&  Serialize(std::ostream& ostream_, std::list<T*>& container)
{

	int size = container.size();
	Serialize(ostream_, size);

	for (auto ite = container.begin(); ite != container.end(); ite++)
	{
		if ((*ite) != NULL)
		{
			bool noNULL = true;
			Serialize(ostream_, noNULL);
			ISerializable* i = dynamic_cast<ISerializable*>(*ite);
			Serialize(ostream_, i);
		}
		else
		{
			bool noNULL = false;
			Serialize(ostream_, noNULL);
		}

	}
	return ostream_;
}



template <class T >
static  std::istream& DeSerialize(std::istream& istream_, std::list<T*>& container)
{
	if (!istream_.good() || istream_.eof())return istream_;
	int size;
	container.clear();

	DeSerialize(istream_, size);

	for (int i = 0; i < size; ++i)
	{
		bool notNULL = false;
		DeSerialize(istream_, notNULL);
		if (notNULL)
		{
			T* object = new T;
			DeSerialize(istream_, dynamic_cast<ISerializable*>(object));
			container.push_back(object);
		}
		else
		{
			container.push_back(NULL);
		}
	}
	return istream_;
}

};