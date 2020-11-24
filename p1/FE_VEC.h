/// @author Simon Gawlok

#ifndef _FE_VEH_H_
#define _FE_VEH_H_

#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include "grid.h"

/// @brief Vector class for point data on a GRID.
class FE_VEC
{
private:
	/// Values of the vector
	std::vector<double> values_;
	
	/// Name of the vector. Is written to visualization output such that
	/// the data of this vector are accessible by this name in visualization
	/// tools (e.g. ParaView)
	char *name_;

public:

	/// Default constructor
	FE_VEC(void)
	{
		/// Set default name to "Vector"
		name_ = (char*) "Vector";
		values_.clear();
	}

	/// Construct vector with a given size. Values are initialized to zero.
	FE_VEC(int size)
	{
		name_ = (char*) "Vector";
		values_.clear();
		values_.resize(size, 0.0);
	}

	/// Construct vector as copy of vec
	FE_VEC(FE_VEC& vec)
	{
		this->name_ = vec.getName();
		this->values_ = vec.getValues();
	}

	/// Destructor
	~FE_VEC(void)
	{
		values_.clear();
		//name = "";
	}

	/// Set name of vector
	inline void setName( char *Name )
	{
		name_ = Name;
	}

	/// Get name of Vector
	inline char* getName( void ) const
	{
		return name_;
	}

	/// Get std::vector with values of this vector
	inline std::vector<double>& getValues( void )
	{
		return values_;
	}

	/// Get length of this vector, i.e. number of elements in values_
	inline int length( void ) const
	{
		return static_cast<int>(values_.size());
	}

	/// Resize vector. If newSize is smaller than the current size, the last
	/// elements will be deleted to fit the new size. If newSize is greater
	/// than the current size, new entries will be initialized with 0.
	inline void resize(int newSize)
	{
		values_.resize(newSize, 0.0);
	}

	/// Set values in vector at given indices
	/// @param values new values to be set
	/// @param indices indices of new values
	template<class T>
	inline void setValues(std::vector<T>& values, std::vector<int>& indices)
	{
		assert(values.size() == indices.size());
		assert(indices.size() <= values_.size());
		
		// set values
		for(int i = 0; i < static_cast<int>(indices.size()); ++i) {
			assert(indices[i] >= 0);
			assert(indices[i] < static_cast<int>(values_.size()));
			
			values_[indices[i]] = static_cast<double>(values[i]);
		}
	}

	/// Access component index
	inline double& operator[](int index)
	{
		assert(index >= 0);
		assert(index < static_cast<int>(values_.size()));
		return values_[index];
	}

	/// Access component index
	inline const double& operator[](int index) const
	{
		assert(index >= 0);
		assert(index < static_cast<int>(values_.size()));
		return values_[index];
	}

	/// Print out vector component by component
	inline void print( void ) const
	{
		std::cout << name_ << ":" << std::endl;
		for(int i = 0; i < static_cast<int>(values_.size()); ++i) {
			std::cout << "\t" << i << ": " << values_[i] << std::endl;
		}
	}
};

#endif
