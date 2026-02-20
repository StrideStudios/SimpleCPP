#pragma once

#include <string>

#ifdef USING_SIMPLESTL
#include "sstl/Container.h"
#endif

#ifdef USING_SIMPLEPTR
#include "sptr/Memory.h"
#endif

class CInputArchive {

protected:

	virtual void read(void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~CInputArchive() = default;

	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inValue) {
		inArchive.read(&inValue, sizeof(TType), 1);
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		EnumType value;
		inArchive >> value;
		inEnum = static_cast<TType>(value);
		return inArchive;
	}

	template <typename TType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TType*& ptr) {
		size_t value;
		inArchive >> value;
		ptr = reinterpret_cast<TType*>(value);
		return inArchive;
	}

	friend CInputArchive& operator>>(CInputArchive& inArchive, std::string& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size);
		inArchive.read(inValue.data(), 1, inValue.size());
		return inArchive;
	}

#ifdef USING_SIMPLEPTR
	template <typename TType,
		std::enable_if_t<std::is_default_constructible_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TShared<TType>& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_default_constructible_v<TType>, int> = 0
	>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TUnique<TType>& inValue) {
		inArchive >> *inValue.get();
		return inArchive;
	}
#endif

#ifdef USING_SIMPLESTL
	template <typename TType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSequenceContainer<TType>& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&](size_t) {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	template <typename TType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TSingleAssociativeContainer<TType>& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TType obj;
			inArchive >> obj;
			return obj;
		});
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend CInputArchive& operator>>(CInputArchive& inArchive, TAssociativeContainer<TKeyType, TValueType>& inValue) {
		size_t size;
		inArchive >> size;
		inValue.resize(size, [&] {
			TPair<TKeyType, TValueType> pair;
			inArchive >> pair.key;
			inArchive >> pair.value;
			return pair;
		});
		return inArchive;
	}
#endif
};

class COutputArchive {

protected:

	virtual void write(const void* inValue, size_t inElementSize, size_t inCount) = 0;

public:

	virtual ~COutputArchive() = default;
	
	template <typename TType,
		std::enable_if_t<std::is_arithmetic_v<TType>, int> = 0
	>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inValue) {
		inArchive.write(&inValue, sizeof(TType), 1);
		return inArchive;
	}

	template <typename TType,
		std::enable_if_t<std::is_enum_v<TType>, int> = 0
	>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType& inEnum) {
		using EnumType = std::underlying_type_t<TType>;
		inArchive << static_cast<EnumType>(inEnum);
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TType*& ptr) {
		inArchive << reinterpret_cast<size_t>(ptr);
		return inArchive;
	}

	friend COutputArchive& operator<<(COutputArchive& inArchive, const std::string& inValue) {
		inArchive << inValue.size();
		inArchive.write(inValue.data(), 1, inValue.size());
		return inArchive;
	}

#ifdef USING_SIMPLEPTR
	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TShared<TType>& inValue) {
		inArchive << *inValue.get();
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TUnique<TType>& inValue) {
		inArchive << *inValue.get();
		return inArchive;
	}
#endif

#ifdef USING_SIMPLESTL
	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSequenceContainer<TType>& inValue) {
		inArchive << inValue.getSize();
		inValue.forEach([&](size_t, const TType& obj) {
			inArchive << obj;
		});
		return inArchive;
	}

	template <typename TType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TSingleAssociativeContainer<TType>& inValue) {
		inArchive << inValue.getSize();
		inValue.forEach([&](const TType& obj) {
			inArchive << obj;
		});
		return inArchive;
	}

	template <typename TKeyType, typename TValueType>
	friend COutputArchive& operator<<(COutputArchive& inArchive, const TAssociativeContainer<TKeyType, TValueType>& inValue) {
		inArchive << inValue.getSize();
		inValue.forEach([&](TPair<TKeyType, const TValueType&> pair) {
			inArchive << pair.key;
			inArchive << pair.value;
		});
		return inArchive;
	}
#endif

};

class CArchive : public CInputArchive, public COutputArchive {};