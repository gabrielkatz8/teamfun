#pragma once
#include <string>
#include <utility>
#include <functional>
#include <iostream>

template <typename ValueType, 
	typename HashFuncType = std::hash<std::string>>
class HashTable
{
public:
	// Constructs the hash table with the given capacity (size of underlying array)
	HashTable(size_t capacity)
		:mTableData(new std::pair<std::string, ValueType>[capacity])
		,mCapacity(capacity)
		,mSize(0)
	{ }

	// TODO: Implement "Rule of five" functions
    ~HashTable() {
        delete [] mTableData;
        mCapacity = 0;
        mSize = 0;
        mTableData = nullptr;
    }
    
    HashTable(const HashTable &h) {
        mTableData = new std::pair<std::string, ValueType>[h.mCapacity];
        std::copy(h.mTableData, &h.mTableData[h.mCapacity - 1], mTableData);
        mCapacity = h.mCapacity;
        mSize = h.mSize;
    }
    
    HashTable(HashTable&& h) noexcept
    : mTableData(std::move(h.mTableData)),
    mCapacity(std::move(h.mCapacity)),
    mSize(std::move(h.mSize)) {
        h.mTableData = nullptr;
        h.mCapacity = 0;
        h.mSize = 0;
    }
    
    HashTable& operator=(const HashTable &h) {
        mTableData = new std::pair<std::string, ValueType>[h.mCapacity];
        std::copy(h.mTableData, &h.mTableData[h.mCapacity - 1], mTableData);
        mCapacity = h.mCapacity;
        mSize = h.mSize;
        return *this;
    }
    
    HashTable& operator=(HashTable&& h) noexcept {
        mTableData = std::move(h.mTableData);
        mCapacity = std::move(h.mCapacity);
        mSize = std::move(h.mSize);
        h.mTableData = nullptr;
        h.mCapacity = 0;
        h.mSize = 0; 
        //std::swap(mTableData, h.mTableData);
        return *this;
    }
    
    

	// Returns overall capacity of hash table (size of underlying array)
	size_t Capacity() const
	{
		return mCapacity;
	}

	// Returns the number of elements currently inserted in the hash table
	size_t Size() const
	{
		return mSize;
	}

	// Tries to insert (key, value) into hash table
	// Returns true if insert succeeds, false if not
	bool Insert(const std::string& key, const ValueType& value)
	{
		// TODO: Implement
        if (mSize == mCapacity) {
            return false;
        }
        
        size_t hashFuncResult = mHashFunc(key) % mCapacity;

        for (size_t i = 0; i < mCapacity; i++) {
            if (hashFuncResult == mCapacity) {
                hashFuncResult = 0;
            }

            if (mTableData[hashFuncResult].first.empty()) {
                mTableData[hashFuncResult] = std::make_pair(key, value);
                mSize++;
                return true;
            }

            hashFuncResult++;
        }
        
        
		return false;
	}

	// Returns pointer to value for given key, or nullptr
	// if not in hash table
	ValueType* Find(const std::string& key) const
	{
		// TODO: Implement
        size_t hashFuncResult = mHashFunc(key) % mCapacity;
        size_t i;
        for (i = 0; i < mCapacity; i++) {
            if (hashFuncResult == mCapacity) {
                hashFuncResult = 0;
            }
            
            if (mTableData[hashFuncResult].first.empty()) {
                return nullptr;
            }
            
            if (mTableData[hashFuncResult].first == key) {
                return &mTableData[hashFuncResult].second;
            }
            
            hashFuncResult++;
        }
        
		return nullptr;
	}

	// Executes provided lambda expression on each element inserted into the table
	void ForEach(std::function<void(const std::string& key, ValueType& value)> func)
	{
		// TODO: Implement
        for (int i = 0; i < mCapacity; i++) {
            if (!mTableData[i].first.empty()) {
                func(mTableData[i].first, mTableData[i].second);
            }
        }
	}

	// Gets a const pointer to underlying data (for testing)
	const std::pair<std::string, ValueType>* Data() const
	{
		return mTableData;
	}
private:
	// NOTE: you should not need any more member data!
	
	// Underlying hash table array
	std::pair<std::string, ValueType>* mTableData;
	// Allocated capacity of hash table array
	size_t mCapacity;
	// Number of elements in HashTable
	size_t mSize;
	// Hash function
	HashFuncType mHashFunc;
};
