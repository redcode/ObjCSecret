// ObjCSecret v1.0
// Copyright (C) 2017-2018 Manuel Sainz de Baranda y Goñi.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// UNALTERED in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef _ObjCSecret_HPP_
#define _ObjCSecret_HPP_

#ifndef OBJC_SECRET_KEY
#	define OBJC_SECRET_KEY 0x7766554433221100
#endif

#import <stdint.h>
#import <stddef.h>
#import <objc/runtime.h>
#import <Foundation/Foundation.h>

#define OBJC_SECRET_KEY_LITERAL(literal) UINT64_C(literal)


template <size_t I> class ObjCSecretCharacter : public ObjCSecretCharacter<I - 1> {
	private:
	char _value;

	public:
	constexpr ObjCSecretCharacter(const char *name) :
	ObjCSecretCharacter<I - 1>(name),
	_value(name[I - 1] ^ (OBJC_SECRET_KEY_LITERAL(OBJC_SECRET_KEY) >> (8 * ((I - 1) % 8))))
		{}
};


template <> class ObjCSecretCharacter<0> {
	public:
	constexpr ObjCSecretCharacter(const char *name) {}
};


template <size_t S> class ObjCSecret : ObjCSecretCharacter<S> {
	public:
	constexpr ObjCSecret(const char *name) : ObjCSecretCharacter<S>(name) {}

	inline void decipher(char *output) const
		{
		for (size_t index = 0; index < S; index++) output[index] =
			reinterpret_cast<const uint8_t *>(this)[index] ^
			(uint8_t)(OBJC_SECRET_KEY_LITERAL(OBJC_SECRET_KEY) >> (8 * (index % 8)));

		output[S] = '\0';
		}


	inline operator NSString *() const
		{
		char buffer[S + 1];
		decipher(buffer);
		return [NSString stringWithUTF8String: buffer];
		}


	inline operator SEL() const
		{
		char buffer[S + 1];
		decipher(buffer);
		return sel_registerName(buffer);
		}


	inline operator id() const
		{
		char buffer[S + 1];
		decipher(buffer);
		return objc_getClass(buffer);
		}
};


#define OBJC_SECRET(name) ObjCSecret<sizeof(#name) - 1>(#name)

#endif // _ObjCSecret_HPP_
