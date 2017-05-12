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
	}
	~iArraySmartPointer()
	{
		if (Array)
		{
			DecrementIndex(Index);
		}
	}
	void IncrementIndex(unsigned int index)
	{
		if (!incremented)
		{
			if (Array)
			{
				Array->IncrementIndex(index);
				incremented = true;
			}
		}
	}
	void DecrementIndex(unsigned int index) {
		if (incremented)
		{
			if (Array)
			{
				Array->DecrementIndex(index);
				incremented = false;
			}
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
	};

public:
	ArraySmartPointer()
	{
	};

	ArraySmartPointer(target const & other)
	{
		*this = ClassArray<target>::Singleton.AddObj(other);
	};
	ArraySmartPointer(target && other)
	{
		*this = ClassArray<target>::Singleton.AddObj(other);
	};

	ArraySmartPointer<target>& operator=(const target& other)
	{
		*this = ClassArray<target>::Singleton.AddObj(other);
		return *this;
	}
/*
	ArraySmartPointer(const ArraySmartPointer & Other)
	{
		if (this != &Other)
			*this = Other;
	};

	ArraySmartPointer(ArraySmartPointer && Other)
	{
		if (this != &Other)
			*this = Other;
	};
	*/
	const ArraySmartPointer& operator=(const ArraySmartPointer& Other)
	{
		Array = Other.Array;
		Index = Other.Index;
		IncrementIndex(Index);
		return *this;
	};

#if false

	template <typename othertarget>	ArraySmartPointer(ArraySmartPointer<othertarget>&& Other)
	{
		if (this != &Other)
			*this = Other;
	}

	template <typename othertarget>	ArraySmartPointer(const ArraySmartPointer<othertarget> & Other)
	{
		if (this != &Other)
			*this = Other;
	}

	template <typename othertarget> ArraySmartPointer<target>& operator=(const ArraySmartPointer<othertarget>& Other)
	{
		target* test = (target*)(Other.operator->());
		if (test)
		{
			Array = Other.Array;
			Index = Other.Index;
			IncrementIndex(Index);
		}

		return *this;
	}
	template <typename othertarget> ArraySmartPointer& operator=(const ArraySmartPointer<othertarget>&& Other)
	{
		Array = Other.Array;
		Index = Other.Index;

		IncrementIndex(Index);

		return *this;
	}
#endif

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
			Singleton.mVector[NextIndex] = other;

		}
		else
		{
			NextIndex = Singleton.mVector.size();
			Singleton.mVector.push_back(other);
			Singleton.mRefCountVector.push_back(0);
		}

		ArraySmartPointer<contained> out(this, NextIndex);

		return out;

	}
public:
	void* GetIndex(unsigned int index) { return &(mVector[index]); };
	static void Reserve(unsigned int numitems) { Singleton.mVector.reserve(numitems); Singleton.mRefCountVector.reserve(numitems); }
	static ArraySmartPointer<contained> GetNew()
	{
		return Singleton.AddObj(contained());
	}
};