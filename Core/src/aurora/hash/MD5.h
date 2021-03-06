#pragma once

#include "aurora/Global.h"

namespace aurora::hash {
	class AE_CORE_DLL MD5 {
	public:
		MD5();
		std::string AE_CALL calc(const void* input, size_t length);

	private:
		void AE_CALL init();
		void AE_CALL update(const uint8_t* buf, size_t length);
		MD5& AE_CALL finalize();
		std::string AE_CALL hexdigest() const;

		inline static const uint32_t BLOCK_SIZE = 64;

		void AE_CALL transform(const uint8_t block[BLOCK_SIZE]);
		static void AE_CALL decode(uint32_t output[], const uint8_t input[], size_t len);
		static void AE_CALL encode(uint8_t output[], const uint32_t input[], size_t len);

		uint8_t _buffer[BLOCK_SIZE];
		uint32_t _count[2];
		uint32_t _state[4];
		uint8_t _digest[16];

		inline static uint32_t AE_CALL F(uint32_t x, uint32_t y, uint32_t z) {
			return (x & y) | ((~x) & z);
		}
		inline static uint32_t AE_CALL G(uint32_t x, uint32_t y, uint32_t z) {
			return (x & z) | (y & ~z);
		}
		inline static uint32_t AE_CALL H(uint32_t x, uint32_t y, uint32_t z) {
			return x ^ y ^ z;
		}
		inline static uint32_t AE_CALL I(uint32_t x, uint32_t y, uint32_t z) {
			return y ^ (x | ~z);
		}
		inline static uint32_t AE_CALL rotate_left(uint32_t x, int32_t n) {
			return (x << n) | (x >> (32 - n));
		}
		inline static void AE_CALL FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
			a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
		}
		inline static void AE_CALL GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
			a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
		}
		inline static void AE_CALL HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
			a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
		}
		inline static void AE_CALL II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) {
			a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
		}
	};
}
