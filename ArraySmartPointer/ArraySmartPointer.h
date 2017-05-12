#pragma once

#include <vector>
#include <queue>

class iArraySmartPointer;
template <typename contained> class ClassArray;

class __declspec(novtable)iClassArray
{
	friend iArraySmartPointer;
	virtual void IncrementIndex(unsigned int index) = 0;
	virtual void DecrementIndex(unsigned int index) = 0;
public:
	virtual void* GetIndex(unsigned int index) = 0;
};

class __declspec(novtable) iArraySmartPointer
{
protected:
	iClassArray* Array = nullptr;
	unsigned int Index;
	bool incremented = false;
public:
	iArraySmartPointer()
	{

	}
	iArraySmartPointer(const iArraySmartPointer & Other)
	{
		DecrementIndex(Index);
		Array = Other.Array;
		Index = Other.Index;
		IncrementIndex(Index);
	}
	iArraySmartPointer& operator=(const iArraySmartPointer & Other)
	{
		DecrementIndex(Index);
		Array = Other.Array;
		Index = Other.Index;
		IncrementIndex(Index);
		return *this;
	}
	~iArraySmartPointer()
	{
			DecrementIndex(Index);
	}
	void IncrementIndex(unsigned int index)
	{
		if (Array)
		{
			Array->IncrementIndex(index);
			incremented = true;
		}
	}
	void DecrementIndex(unsigned int index) {
		if (Array)
		{
			Array->DecrementIndex(index);
			incremented = false;
		}
	}
};

template <typename target> class ArraySmartPointer : iArraySmartPointer
{
	friend ClassArray<target>;

	ArraySmartPointer(iClassArray* Array, unsigned int Index)
	{
		this->Array = Array;
		this->Index = Index;
		IncrementIndex(Index);
	}

public:

	ArraySmartPointer()
	{
	}

	ArraySmartPointer(const target & other)
	{
		*this = ClassArray<target>::Singleton.AddObj(other);
	};
	//The object in question may contain data worth having a move constructor for.
	ArraySmartPointer(target && other)
	{
		*this = ClassArray<target>::Singleton.AddObj(std::move(other));
	}

	ArraySmartPointer& operator=(const target& other)
	{
		*this = ClassArray<target>::Singleton.AddObj(other);
		return *this;
	}
	//The object in question may contain data worth having a move assignment operator for.
	ArraySmartPointer& operator=(target&& other)
	{

		*this = ClassArray<target>::Singleton.AddObj(std::move(other));
		return *this;
	}

//Calls the parent's copy constructor, which handles all data.
	ArraySmartPointer(const ArraySmartPointer & other) : iArraySmartPointer(other)
	{

	}
	//Call's the parent's assignment operator, which handles all data.
	ArraySmartPointer& operator=(const ArraySmartPointer& other)
	{
		iArraySmartPointer::operator=(other);
		return *this;
	}

	//Returns a pointer to the object in question
	target* operator->()
	{
		if (Array)
		{
			return (target*)Array->GetIndex(Index);
		}
		else
		{
			return nullptr;
		}
	}

};


template <typename contained> class ClassArray : iClassArray
{
	friend ArraySmartPointer<contained>;
	static ClassArray<contained> Singleton;
	std::vector<contained> mVector;
	std::vector<unsigned int> mRefCountVector;
	std::queue<unsigned int> mReusableIDVector;
	virtual void IncrementIndex(unsigned int index) { mRefCountVector[index]++; };
	virtual void DecrementIndex(unsigned int index)
	{
		mRefCountVector[index]--;
		if (mRefCountVector[index] == 0)
		{
			mReusableIDVector.push(index);
		}
	};

	ArraySmartPointer<contained> AddObj(contained const & other)
	{
		unsigned int NextIndex;
		if (Singleton.mReusableIDVector.size() > 0)
		{
			NextIndex = Singleton.mReusableIDVector.front();
			Singleton.mReusableIDVector.pop();
			Singleton.mVector[NextIndex] = other;

		}
		else
		{
			NextIndex = (unsigned int)Singleton.mVector.size();
			Singleton.mVector.push_back(other);
			Singleton.mRefCountVector.push_back(0);
		}

		ArraySmartPointer<contained> out(this, NextIndex);
		return out;
	}

	ArraySmartPointer<contained> AddObj(contained && other)
	{
		unsigned int NextIndex;
		if (Singleton.mReusableIDVector.size() > 0)
		{
			NextIndex = Singleton.mReusableIDVector.front();
			Singleton.mReusableIDVector.pop();
			Singleton.mVector[NextIndex] = std::move(other);

		}
		else
		{
			NextIndex = (unsigned int)Singleton.mVector.size();
			Singleton.mVector.push_back(std::move(other));
			Singleton.mRefCountVector.push_back(0);
		}

		ArraySmartPointer<contained> out(this, NextIndex);

		return out;

	}
public:
	void* GetIndex(unsigned int index) { return &(mVector[index]); };

	static void Reserve(unsigned int numitems) { Singleton.mVector.reserve(numitems); Singleton.mRefCountVector.reserve(numitems); }
	//if object lacks a default constructor then GetNew() isn't an option sadly, but thanks to the magic of templates if we don't call it anywhere this won't try to compile.
	static ArraySmartPointer<contained> GetNew()
	{
		return Singleton.AddObj(contained());
	}
};