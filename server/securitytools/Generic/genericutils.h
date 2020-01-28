////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericUtils_h_included
#define GenericUtils_h_included

namespace SecUtils {

	// PYR-92828
	template <typename T>
	void appendLogVec(const char* vecName, const vector<T>& vec, PString& logStr)
	{
		static const unsigned MAX_LEN = 10;

		logStr.append(vecName).append(": [");

		for (unsigned i = 0; i < vec.size() && i < MAX_LEN; ++i)
		{
			if (i > 0)
			{
				logStr.append(",");
			}
			vec[i].appendLog(logStr);
		}

		if (vec.size() > MAX_LEN)
		{
			int numExcess = vec.size() - MAX_LEN;
			logStr.append(",+ ").appendInt(numExcess).append(" more");
		}

		logStr.append("]");
	}

	template <typename T>
	void appendLogPStringVec(const char* vecName, const vector<T>& vec, PString& logStr)
	{
		static const unsigned MAX_LEN = 10;

		logStr.append(vecName).append(": [");

		for (unsigned i = 0; i < vec.size() && i < MAX_LEN; ++i)
		{
			if (i > 0)
			{
				logStr.append(",");
			}
			logStr.append(vec[i]);
		}

		if (vec.size() > MAX_LEN)
		{
			int numExcess = vec.size() - MAX_LEN;
			logStr.append(",+ ").appendInt(numExcess).append(" more");
		}

		logStr.append("]");
	}

	template <typename T>
	void appendLogIntVec(const char* vecName, const vector<T>& vec, PString& logStr)
	{
		static const unsigned MAX_LEN = 10;

		logStr.append(vecName).append(": [");

		for (unsigned i = 0; i < vec.size() && i < MAX_LEN; ++i)
		{
			if (i > 0)
			{
				logStr.append(",");
			}
			logStr.appendInt(vec[i]);
		}

		if (vec.size() > MAX_LEN)
		{
			int numExcess = vec.size() - MAX_LEN;
			logStr.append(",+ ").appendInt(numExcess).append(" more");
		}

		logStr.append("]");
	}
	
		// PYR-98541
	template<typename Container> using ValueType = typename Container::value_type;
	template<typename Container> using Iterator = typename Container::iterator;
	template<typename Container> using ConstIterator = typename Container::const_iterator;
	
	template<typename Container, typename Pred>
	ConstIterator<Container> find_if(const Container& container, Pred pred)
	{
		return std::find_if(container.begin(), container.end(), pred);
	}

	template<typename Container, typename Pred>
	Iterator<Container> find_if(Container& container, Pred pred)
	{
		return std::find_if(container.begin(), container.end(), pred);
	}

	template<typename Container, typename Pred>
	void for_each(const Container& container, Pred pred)
	{
		std::for_each(container.begin(), container.end(), pred);
	}

	template<typename Container, typename Pred>
	void for_each(Container& container, Pred pred)
	{
		std::for_each(container.begin(), container.end(), pred);
	}

} // namespace

////////////////////////////////////////////////////////////////////////////////

#endif // GenericUtils_h_included
