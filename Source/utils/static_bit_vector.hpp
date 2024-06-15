#include <algorithm>
#include <climits>
#include <cstddef>

#include "utils/attributes.h"

namespace devilution {

/**
 * @brief A stack-allocated bit-vector with a fixed capacity and a fixed size < capacity.
 *
 * @tparam N capacity.
 */
template <size_t N, typename Word = unsigned long> // NOLINT(google-runtime-int)
class StaticBitVector {
public:
	explicit StaticBitVector(size_t size)
	    : size_(size)
	    , num_words_(size / BitsPerWord + ((size % BitsPerWord) == 0 ? 0 : 1))
	{
	}

	[[nodiscard]] bool test(size_t pos) const { return (word(pos) & maskBit(pos)) != 0; }

	void set() { std::fill_n(words_, num_words_, ~Word { 0 }); }
	void set(size_t pos) { word(pos) |= maskBit(pos); }

	// Sets all bits in the [begin, begin+length) range to 1.
	// Length must not be zero.
	void set(size_t begin, size_t length)
	{
		DVL_ASSUME(length != 0);
		const size_t firstWord = begin / BitsPerWord;
		const size_t lastWord = (begin + length - 1) / BitsPerWord;
		const Word firstWordMask = onesFrom(begin);
		const Word lastWordMask = onesUpTo(begin + length - 1);
		if (firstWord == lastWord) {
			words_[firstWord] |= firstWordMask & lastWordMask;
			return;
		}
		words_[firstWord] |= firstWordMask;
		std::fill(&words_[firstWord + 1], &words_[lastWord], ~Word { 0 });
		words_[lastWord] |= lastWordMask;
	}

	void reset() { std::fill_n(words_, num_words_, Word { 0 }); }
	void reset(size_t pos) { word(pos) &= ~maskBit(pos); }

	// Sets all bits in the [begin, begin+length) range to 0.
	// Length must not be zero.
	void reset(size_t begin, size_t length)
	{
		DVL_ASSUME(length != 0);
		const size_t firstWord = begin / BitsPerWord;
		const size_t lastWord = (begin + length - 1) / BitsPerWord;
		const Word firstWordMask = onesFrom(begin);
		const Word lastWordMask = onesUpTo(begin + length - 1);
		if (firstWord == lastWord) {
			words_[firstWord] &= ~(firstWordMask & lastWordMask);
			return;
		}
		words_[firstWord] &= ~firstWordMask;
		std::fill(&words_[firstWord + 1], &words_[lastWord], Word { 0 });
		words_[lastWord] &= ~lastWordMask;
	}

private:
	static constexpr Word onesFrom(size_t begin)
	{
		return (~Word { 0 }) << (begin % BitsPerWord);
	}
	static constexpr Word onesUpTo(size_t last)
	{
		return (~Word { 0 }) >> (BitsPerWord - (last % BitsPerWord) - 1);
	}

	static constexpr Word maskBit(size_t pos)
	{
		return Word { 1 } << (pos % BitsPerWord);
	}
	Word word(size_t pos) const { return words_[pos / BitsPerWord]; }
	Word &word(size_t pos) { return words_[pos / BitsPerWord]; }

	static constexpr unsigned BitsPerWord = CHAR_BIT * sizeof(Word);
	static constexpr unsigned MaxNumWords = N / BitsPerWord + ((N % BitsPerWord) == 0 ? 0 : 1);
	Word words_[MaxNumWords] = {};

	size_t size_;
	size_t num_words_;
};

} // namespace devilution
