// seckey.h - written and placed in the public domain by Wei Dai

// This file contains helper classes/functions for implementing secret key algorithms.

#ifndef CRYPTOPP_SECKEY_H
#define CRYPTOPP_SECKEY_H

#include "cryptlib.h"
#include "misc.h"
#include "simple.h"

NAMESPACE_BEGIN(CryptoPP)

inline CipherDir ReverseCipherDir(CipherDir dir)
{
	return (dir == ENCRYPTION) ? DECRYPTION : ENCRYPTION;
}

//! to be inherited by block ciphers with fixed block size
template <unsigned int N>
class FixedBlockSize
{
public:
	enum {BLOCKSIZE = N};
};

// ************** rounds ***************

//! to be inherited by ciphers with fixed number of rounds
template <unsigned int R>
class FixedRounds
{
public:
	enum {ROUNDS = R};
};

//! to be inherited by ciphers with variable number of rounds
template <unsigned int D, unsigned int N=1, unsigned int M=INT_MAX>		// use INT_MAX here because enums are treated as signed ints
class VariableRounds
{
public:
	enum {DEFAULT_ROUNDS = D, MIN_ROUNDS = N, MAX_ROUNDS = M};
	static unsigned int StaticGetDefaultRounds(size_t keylength) {return DEFAULT_ROUNDS;}

protected:
	static inline void AssertValidRounds(unsigned int rounds)
	{
		assert(rounds >= (unsigned int)MIN_ROUNDS && rounds <= (unsigned int)MAX_ROUNDS);
	}

	inline void ThrowIfInvalidRounds(int rounds, const Algorithm *alg)
	{
		if (rounds < (int)MIN_ROUNDS || rounds > (int)MAX_ROUNDS)
			throw InvalidRounds(alg->AlgorithmName(), rounds);
	}

	inline unsigned int GetRoundsAndThrowIfInvalid(const NameValuePairs &param, const Algorithm *alg)
	{
		int rounds = param.GetIntValueWithDefault("Rounds", DEFAULT_ROUNDS);
		ThrowIfInvalidRounds(rounds, alg);
		return (unsigned int)rounds;
	}
};

// ************** key length ***************

//! to be inherited by keyed algorithms with fixed key length
template <unsigned int N, unsigned int IV_REQ = SimpleKeyingInterface::NOT_RESYNCHRONIZABLE>
class FixedKeyLength
{
public:
	enum {KEYLENGTH=N, MIN_KEYLENGTH=N, MAX_KEYLENGTH=N, DEFAULT_KEYLENGTH=N};
	enum {IV_REQUIREMENT = IV_REQ};
	static size_t CRYPTOPP_API StaticGetValidKeyLength(size_t) {return KEYLENGTH;}
};

/// support query of variable key length, template parameters are default, min, max, multiple (default multiple 1)
template <unsigned int D, unsigned int N, unsigned int M, unsigned int Q = 1, unsigned int IV_REQ = SimpleKeyingInterface::NOT_RESYNCHRONIZABLE>
class VariableKeyLength
{
	// make these private to avoid Doxygen documenting them in all derived classes
	CRYPTOPP_COMPILE_ASSERT(Q > 0);
	CRYPTOPP_COMPILE_ASSERT(N % Q == 0);
	CRYPTOPP_COMPILE_ASSERT(M % Q == 0);
	CRYPTOPP_COMPILE_ASSERT(N < M);
	CRYPTOPP_COMPILE_ASSERT(D >= N && M >= D);

public:
	enum {MIN_KEYLENGTH=N, MAX_KEYLENGTH=M, DEFAULT_KEYLENGTH=D, KEYLENGTH_MULTIPLE=Q};
	enum {IV_REQUIREMENT = IV_REQ};
	static size_t CRYPTOPP_API StaticGetValidKeyLength(size_t n)
	{
		if (n < (size_t)MIN_KEYLENGTH)
			return MIN_KEYLENGTH;
		else if (n > (size_t)MAX_KEYLENGTH)
			return (size_t)MAX_KEYLENGTH;
		else
		{
			n += KEYLENGTH_MULTIPLE-1;
			return n - n%KEYLENGTH_MULTIPLE;
		}
	}
};

/// support query of key length that's the same as another class
template <class T>
class SameKeyLengthAs
{
public:
	enum {MIN_KEYLENGTH=T::MIN_KEYLENGTH, MAX_KEYLENGTH=T::MAX_KEYLENGTH, DEFAULT_KEYLENGTH=T::DEFAULT_KEYLENGTH};
	enum {IV_REQUIREMENT = T::IV_REQUIREMENT};
	static size_t CRYPTOPP_API StaticGetValidKeyLength(size_t keylength)
		{return T::StaticGetValidKeyLength(keylength);}
};

// ************** implementation helper for SimpledKeyed ***************

//! _
template <class BASE, class INFO = BASE>
class CRYPTOPP_NO_VTABLE SimpleKeyingInterfaceImpl : public BASE
{
public:
	size_t MinKeyLength() const {return INFO::MIN_KEYLENGTH;}
	size_t MaxKeyLength() const {return (size_t)INFO::MAX_KEYLENGTH;}
	size_t DefaultKeyLength() const {return INFO::DEFAULT_KEYLENGTH;}
	size_t GetValidKeyLength(size_t n) const {return INFO::StaticGetValidKeyLength(n);}
	typename BASE::IV_Requirement IVRequirement() const {return (typename BASE::IV_Requirement)INFO::IV_REQUIREMENT;}
};

template <class INFO, class BASE = BlockCipher>
class CRYPTOPP_NO_VTABLE BlockCipherImpl : public AlgorithmImpl<SimpleKeyingInterfaceImpl<TwoBases<BASE, INFO> > >
{
public:
	unsigned int BlockSize() const {return this->BLOCKSIZE;}
};

//! _
template <CipherDir DIR, class BASE>
class BlockCipherFinal : public ClonableImpl<BlockCipherFinal<DIR, BASE>, BASE>
{
public:
 	BlockCipherFinal() {}
	BlockCipherFinal(const byte *key)
		{this->SetKey(key, this->DEFAULT_KEYLENGTH);}
	BlockCipherFinal(const byte *key, size_t length)
		{this->SetKey(key, length);}
	BlockCipherFinal(const byte *key, size_t length, unsigned int rounds)
		{this->SetKeyWithRounds(key, length, rounds);}

	bool IsForwardTransformation() const {return DIR == ENCRYPTION;}
};

//! _
template <class BASE, class INFO = BASE>
class MessageAuthenticationCodeImpl : public AlgorithmImpl<SimpleKeyingInterfaceImpl<BASE, INFO>, INFO>
{
};

//! _
template <class BASE>
class MessageAuthenticationCodeFinal : public ClonableImpl<MessageAuthenticationCodeFinal<BASE>, MessageAuthenticationCodeImpl<BASE> >
{
public:
 	MessageAuthenticationCodeFinal() {}
	MessageAuthenticationCodeFinal(const byte *key)
		{this->SetKey(key, this->DEFAULT_KEYLENGTH);}
	MessageAuthenticationCodeFinal(const byte *key, size_t length)
		{this->SetKey(key, length);}
};

// ************** documentation ***************

//! These objects usually should not be used directly. See CipherModeDocumentation instead.
/*! Each class derived from this one defines two types, Encryption and Decryption, 
	both of which implement the BlockCipher interface. */
struct BlockCipherDocumentation
{
	//! implements the BlockCipher interface
	typedef BlockCipher Encryption;
	//! implements the BlockCipher interface
	typedef BlockCipher Decryption;
};

/*! \brief Each class derived from this one defines two types, Encryption and Decryption, 
	both of which implement the SymmetricCipher interface. Two types of classes derive
	from this class: stream ciphers and block cipher modes. Stream ciphers can be used
	alone, cipher mode classes need to be used with a block cipher. See CipherModeDocumentation
	for more for information about using cipher modes and block ciphers. */
struct SymmetricCipherDocumentation
{
	//! implements the SymmetricCipher interface
	typedef SymmetricCipher Encryption;
	//! implements the SymmetricCipher interface
	typedef SymmetricCipher Decryption;
};

NAMESPACE_END

#endif
