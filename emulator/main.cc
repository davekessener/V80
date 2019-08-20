#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdint.h>
#include <string.h>

#define FOLD(e) ((void) ::std::initializer_list<int>{0, ((void) e, 0)... })

typedef uint8_t  u8;
typedef  int8_t  i8;
typedef uint16_t u16;
typedef  int16_t i16;
typedef uint32_t u32;
typedef  int32_t i32;
typedef uint64_t u64;
typedef  int64_t i64;

typedef uint8_t byte_t;
typedef unsigned uint;

#define FLAG_CARRY 1
#define FLAG_ZERO 2
#define FLAG_NEGATIVE 4

template<typename S>
void stringify_impl(S&)
{
}

template<typename S, typename T, typename ... TT>
void stringify_impl(S& ss, const T& v, const TT& ... o)
{
	ss << v;

	stringify_impl(ss, o...);
}

template<typename ... T>
std::string stringify(const T& ... o)
{
	std::stringstream ss;

	stringify_impl(ss, o...);

	return ss.str();
}

template<size_t N, typename T>
std::string hex(const T& v)
{
	return stringify(std::hex, std::setw(N / 4), std::setfill('0'), (u64) v);
}

namespace v80
{
	static std::string disassemble(const u8 *p)
	{
		switch(*p)
		{
			case 0x00:
				return "nop";

			case 0x01:
				return "halt";

			case 0x02:
				return "ei";

			case 0x03:
				return "di";

			case 0x04:
				return "int";

			case 0x05:
				return "rst";

			case 0x06:
			case 0x07:
				return "EX"; // TODO

			case 0x08:
				return stringify("ld fa,0x", hex<16>((p[2] << 8) | p[1]));

			case 0x09:
				return stringify("ld bc,0x", hex<16>((p[2] << 8) | p[1]));

			case 0x0A:
				return stringify("ld de,0x", hex<16>((p[2] << 8) | p[1]));

			case 0x0B:
				return stringify("ld hl,0x", hex<16>((p[2] << 8) | p[1]));

			case 0x0C:
			case 0x0D:
			case 0x0E:
			case 0x0F:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
				return "UNKNOWN";

			case 0x18:
				return stringify("ld (hl),0x", hex<8>(p[1]));

			case 0x19:
				return stringify("ld a,0x", hex<8>(p[1]));

			case 0x1A:
				return stringify("ld b,0x", hex<8>(p[1]));

			case 0x1B:
				return stringify("ld c,0x", hex<8>(p[1]));

			case 0x1C:
				return stringify("ld d,0x", hex<8>(p[1]));

			case 0x1D:
				return stringify("ld e,0x", hex<8>(p[1]));

			case 0x1E:
				return stringify("ld h,0x", hex<8>(p[1]));

			case 0x1F:
				return stringify("ld l,0x", hex<8>(p[1]));

			case 0x20:
				return stringify("add ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x21:
				return stringify("adc ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x22:
				return stringify("sub ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x23:
				return stringify("sbc ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x24:
				return stringify("and ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x25:
				return stringify("or ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x26:
				return stringify("xor ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x27:
				return stringify("cp ", (int) p[1], " [0x", hex<8>(p[1]), "]");

			case 0x28:
				return "dec af";

			case 0x29:
				return "dec bc";

			case 0x2A:
				return "dec de";

			case 0x2B:
				return "dec hl";

			case 0x2C:
				return "inc af";

			case 0x2D:
				return "inc bc";

			case 0x2E:
				return "inc de";

			case 0x2F:
				return "inc hl";

			case 0x30:
				return "push af";

			case 0x31:
				return "push bc";

			case 0x32:
				return "push de";

			case 0x33:
				return "push hl";

			case 0x34:
				return "pop af";

			case 0x35:
				return "pop bc";

			case 0x36:
				return "pop de";

			case 0x37:
				return "pop hl";

			case 0x38:
				return "ret c";

			case 0x39:
				return "ret z";

			case 0x3A:
				return "ret n";

			case 0x3B:
				return "ret";

			case 0x3C:
				return "ret nc";

			case 0x3D:
				return "ret nz";

			case 0x3E:
				return "ret p";

			case 0x3F:
				return "INVALID";

			case 0x40:
				return "ld af,af";

			case 0x41:
				return "ld af,bc";

			case 0x42:
				return "ld af,de";

			case 0x43:
				return "ld af,hl";

			case 0x44:
				return "ld bc,af";

			case 0x45:
				return "ld bc,bc";

			case 0x46:
				return "ld bc,de";

			case 0x47:
				return "ld bc,hl";

			case 0x48:
				return "ld de,af";

			case 0x49:
				return "ld de,bc";

			case 0x4A:
				return "ld de,de";

			case 0x4B:
				return "ld de,hl";

			case 0x4C:
				return "ld hl,af";

			case 0x4D:
				return "ld hl,bc";

			case 0x4E:
				return "ld hl,de";

			case 0x4F:
				return "ld hl,hl";

			case 0x50:
				return "out [(hl],a";

			case 0x51:
				return "out [a],a";

			case 0x52:
				return "out [b],a";

			case 0x53:
				return "out [c],a";

			case 0x54:
				return "out [d],a";

			case 0x55:
				return "out [e],a";

			case 0x56:
				return "out [h],a";

			case 0x57:
				return "out [l],a";

			case 0x58:
				return "in a,[(hl)]";

			case 0x59:
				return "in a,[a]";

			case 0x5A:
				return "in a,[b]";

			case 0x5B:
				return "in a,[c]";

			case 0x5C:
				return "in a,[d]";

			case 0x5D:
				return "in a,[e]";

			case 0x5E:
				return "in a,[h]";

			case 0x5F:
				return "in a,[l]";

			case 0x60:
				return stringify("jp c,$", hex<16>((p[2] << 8) | p[1]));

			case 0x61:
				return stringify("jp z,$", hex<16>((p[2] << 8) | p[1]));

			case 0x62:
				return stringify("jp n,$", hex<16>((p[2] << 8) | p[1]));

			case 0x63:
				return stringify("jp $", hex<16>((p[2] << 8) | p[1]));

			case 0x64:
				return stringify("jp nc,$", hex<16>((p[2] << 8) | p[1]));

			case 0x65:
				return stringify("jp nz,$", hex<16>((p[2] << 8) | p[1]));

			case 0x66:
				return stringify("jp p,$", hex<16>((p[2] << 8) | p[1]));

			case 0x67:
				return "INVALID";

			case 0x68:
				return stringify("call c,$", hex<16>((p[2] << 8) | p[1]));

			case 0x69:
				return stringify("call z,$", hex<16>((p[2] << 8) | p[1]));

			case 0x6A:
				return stringify("call n,$", hex<16>((p[2] << 8) | p[1]));

			case 0x6B:
				return stringify("call $", hex<16>((p[2] << 8) | p[1]));

			case 0x6C:
				return stringify("call nc,$", hex<16>((p[2] << 8) | p[1]));

			case 0x6D:
				return stringify("call nz,$", hex<16>((p[2] << 8) | p[1]));

			case 0x6E:
				return stringify("call p,$", hex<16>((p[2] << 8) | p[1]));

			case 0x6F:
				return "INVALID";

			case 0x70:
				return "dec (hl)";

			case 0x71:
				return "dec a";

			case 0x72:
				return "dec b";

			case 0x73:
				return "dec c";

			case 0x74:
				return "dec d";

			case 0x75:
				return "dec e";

			case 0x76:
				return "dec h";

			case 0x77:
				return "dec l";

			case 0x78:
				return "inc (hl)";

			case 0x79:
				return "inc a";

			case 0x7A:
				return "inc b";

			case 0x7B:
				return "inc c";

			case 0x7C:
				return "inc d";

			case 0x7D:
				return "inc e";

			case 0x7E:
				return "inc h";

			case 0x7F:
				return "inc l";

			case 0x80:
				return "add (hl)";

			case 0x81:
				return "add a";

			case 0x82:
				return "add b";

			case 0x83:
				return "add c";

			case 0x84:
				return "add d";

			case 0x85:
				return "add e";

			case 0x86:
				return "add h";

			case 0x87:
				return "add l";

			case 0x88:
				return "adc (hl)";

			case 0x89:
				return "adc a";

			case 0x8A:
				return "adc b";

			case 0x8B:
				return "adc c";

			case 0x8C:
				return "adc d";

			case 0x8D:
				return "adc e";

			case 0x8E:
				return "adc h";

			case 0x8F:
				return "adc l";

			case 0x90:
				return "sub (hl)";

			case 0x91:
				return "sub a";

			case 0x92:
				return "sub b";

			case 0x93:
				return "sub c";

			case 0x94:
				return "sub d";

			case 0x95:
				return "sub e";

			case 0x96:
				return "sub h";

			case 0x97:
				return "sub l";

			case 0x98:
				return "sbc (hl)";

			case 0x99:
				return "sbc a";

			case 0x9A:
				return "sbc b";

			case 0x9B:
				return "sbc c";

			case 0x9C:
				return "sbc d";

			case 0x9D:
				return "sbc e";

			case 0x9E:
				return "sbc h";

			case 0x9F:
				return "sbc l";

			case 0xA0:
				return "and (hl)";

			case 0xA1:
				return "and a";

			case 0xA2:
				return "and b";

			case 0xA3:
				return "and c";

			case 0xA4:
				return "and d";

			case 0xA5:
				return "and e";

			case 0xA6:
				return "and h";

			case 0xA7:
				return "and l";

			case 0xA8:
				return "or (hl)";

			case 0xA9:
				return "or a";

			case 0xAA:
				return "or b";

			case 0xAB:
				return "or c";

			case 0xAC:
				return "or d";

			case 0xAD:
				return "or e";

			case 0xAE:
				return "or h";

			case 0xAF:
				return "or l";

			case 0xB0:
				return "xor (hl)";

			case 0xB1:
				return "xor a";

			case 0xB2:
				return "xor b";

			case 0xB3:
				return "xor c";

			case 0xB4:
				return "xor d";

			case 0xB5:
				return "xor e";

			case 0xB6:
				return "xor h";

			case 0xB7:
				return "xor l";

			case 0xB8:
				return "cp (hl)";

			case 0xB9:
				return "cp a";

			case 0xBA:
				return "cp b";

			case 0xBB:
				return "cp c";

			case 0xBC:
				return "cp d";

			case 0xBD:
				return "cp e";

			case 0xBE:
				return "cp h";

			case 0xBF:
				return "cp l";

			case 0xC0:
				return "ld (hl),(hl)";

			case 0xC1:
				return "ld (hl),a";

			case 0xC2:
				return "ld (hl),b";

			case 0xC3:
				return "ld (hl),c";

			case 0xC4:
				return "ld (hl),d";

			case 0xC5:
				return "ld (hl),e";

			case 0xC6:
				return "ld (hl),h";

			case 0xC7:
				return "ld (hl),l";

			case 0xC8:
				return "ld a,(hl)";

			case 0xC9:
				return "ld a,a";

			case 0xCA:
				return "ld a,b";

			case 0xCB:
				return "ld a,c";

			case 0xCC:
				return "ld a,d";

			case 0xCD:
				return "ld a,e";

			case 0xCE:
				return "ld a,h";

			case 0xCF:
				return "ld a,l";

			case 0xD0:
				return "ld b,(hl)";

			case 0xD1:
				return "ld b,a";

			case 0xD2:
				return "ld b,b";

			case 0xD3:
				return "ld b,c";

			case 0xD4:
				return "ld b,d";

			case 0xD5:
				return "ld b,e";

			case 0xD6:
				return "ld b,h";

			case 0xD7:
				return "ld b,l";

			case 0xD8:
				return "ld c,(hl)";

			case 0xD9:
				return "ld c,a";

			case 0xDA:
				return "ld c,b";

			case 0xDB:
				return "ld c,c";

			case 0xDC:
				return "ld c,d";

			case 0xDD:
				return "ld c,e";

			case 0xDE:
				return "ld c,h";

			case 0xDF:
				return "ld c,l";

			case 0xE0:
				return "ld d,(hl)";

			case 0xE1:
				return "ld d,a";

			case 0xE2:
				return "ld d,b";

			case 0xE3:
				return "ld d,c";

			case 0xE4:
				return "ld d,d";

			case 0xE5:
				return "ld d,e";

			case 0xE6:
				return "ld d,h";

			case 0xE7:
				return "ld d,l";

			case 0xE8:
				return "ld e,(hl)";

			case 0xE9:
				return "ld e,a";

			case 0xEA:
				return "ld e,b";

			case 0xEB:
				return "ld e,c";

			case 0xEC:
				return "ld e,d";

			case 0xED:
				return "ld e,e";

			case 0xEE:
				return "ld e,h";

			case 0xEF:
				return "ld e,l";

			case 0xF0:
				return "ld h,(hl)";

			case 0xF1:
				return "ld h,a";

			case 0xF2:
				return "ld h,b";

			case 0xF3:
				return "ld h,c";

			case 0xF4:
				return "ld h,d";

			case 0xF5:
				return "ld h,e";

			case 0xF6:
				return "ld h,h";

			case 0xF7:
				return "ld h,l";

			case 0xF8:
				return "ld l,(hl)";

			case 0xF9:
				return "ld l,a";

			case 0xFA:
				return "ld l,b";

			case 0xFB:
				return "ld l,c";

			case 0xFC:
				return "ld l,d";

			case 0xFD:
				return "ld l,e";

			case 0xFE:
				return "ld l,h";

			case 0xFF:
				return "ld l,l";

			default:
				return "INVALID";
		}
	}

	template<typename ... T>
	class Periphery : public T...
	{
		typedef void (Periphery::* out_fn)(u8);
		typedef u8 (Periphery::* in_fn)(void);

		public:
			Periphery( );
			void start();
			void stop();
			u32 tick();
			void out(u8, u8);
			void print(std::ostream&) const;
			u8 in(u8);

		private:
			out_fn mOut[0x100];
			in_fn mIn[0x100];
	};

	template<typename ... T>
	Periphery<T...>::Periphery(void)
		: mOut{ (&T::out)... }
		, mIn{ (&T::in)... }
	{
	}

	template<typename ... T>
	void Periphery<T...>::start(void)
	{
		FOLD(T::start());
	}

	template<typename ... T>
	void Periphery<T...>::stop(void)
	{
		FOLD(T::stop());
	}

	template<typename ... T>
	u32 Periphery<T...>::tick(void)
	{
		u32 ints = 0;
		u8 f[] = { (u8) (T::tick() ? 1 : 0)... };

		for(uint i = 0 ; i < sizeof(f) ; ++i)
		{
			if(f[i])
				ints |= 1 << i;
		}

		return ints;
	}

	template<typename ... T>
	void Periphery<T...>::out(u8 port, u8 v)
	{
		(this->*mOut[port])(v);
	}

	template<typename ... T>
	u8 Periphery<T...>::in(u8 port)
	{
		return (this->*mIn[port])();
	}

	template<typename S>
	void printPeriphery(S&, const void *, u8)
	{
	}

	template<typename S, typename T, typename ... TT>
	void printPeriphery(S& os, const void *self, u8 i)
	{
		os << "# --- P0x" << hex<8>(i) << " - " << ((const T *) self)->status() << "\n";

		printPeriphery<S, TT...>(os, self, i + 1);
	}

	template<typename ... T>
	void Periphery<T...>::print(std::ostream& os) const
	{
		printPeriphery<decltype(os), T...>(os, this, 0);
	}

	struct BasePeriphery
	{
		void start( ) { }
		void stop( ) { }
		bool tick( ) { return false; }
		void out(u8) { }
		u8 in( ) { return -1; }
	};

	template
	<
		typename P
	>
	class System : P
	{
		typedef System<P> Self;
		typedef void (System::* ins_fn)(u8);

		public:
			System( );
			bool running( ) const { return mRunning; }
			void start( );
			void stop( );
			void load(u16, u16, const byte_t *);
			void step( );

			void print(std::ostream&) const;
			void printMemory(std::ostream&, u16, u16) const;

		private:
			void push(u16 v)
				{ mRAM[--mSP] = (v & 0xFF); mRAM[--mSP] = (v >> 8); }
			u16 pop( )
				{ u16 v = mRAM[mSP++] << 8; return v | mRAM[mSP++]; }

			u8 next8( ) { return mRAM[mPC++]; }
			u16 next16( ) { u16 v = next8(); return (next8() << 8) | v; }

			void ins_invalid(u8);
			void ins_nop(u8);
			void ins_halt(u8);
			void ins_ei(u8);
			void ins_di(u8);
			void ins_int(u8);
			void ins_rst(u8);
			void ins_c8(u8);
			void ins_c16(u8);
			void ins_ld16_v(u8);
			void ins_ld8_v(u8);
			void ins_math_v(u8);
			void ins_stack(u8);
			void ins_ret(u8);
			void ins_b(u8);
			void ins_io(u8);
			void ins_ld16(u8);
			void ins_math8(u8);
			void ins_ld8(u8);

			void addIns(u8, u8, ins_fn);

			u8 math(u8, u8, u8);
			bool flag(u8);

			u16& reg16(uint i) { return ((u16 *) mReg)[i]; }

			u8 F() const { return mReg[0]; }
			u8 A() const { return mReg[1]; }
			u8 B() const { return mReg[2]; }
			u8 C() const { return mReg[3]; }
			u8 D() const { return mReg[4]; }
			u8 E() const { return mReg[5]; }
			u8 H() const { return mReg[6]; }
			u8 L() const { return mReg[7]; }

			u16 FA() const { return reg16(0); }
			u16 BC() const { return reg16(1); }
			u16 DE() const { return reg16(2); }
			u16 HL() const { return reg16(3); }

			u8& F() { return mReg[0]; }
			u8& A() { return mReg[1]; }
			u8& B() { return mReg[2]; }
			u8& C() { return mReg[3]; }
			u8& D() { return mReg[4]; }
			u8& E() { return mReg[5]; }
			u8& H() { return mReg[6]; }
			u8& L() { return mReg[7]; }

			u16& FA() { return reg16(0); }
			u16& BC() { return reg16(1); }
			u16& DE() { return reg16(2); }
			u16& HL() { return reg16(3); }

			u8& reg8(uint i) { return i == 0 ? mRAM[HL()] : mReg[i]; }

		private:
			u16 mPC, mSP;
			u8 mReg[8];
			byte_t mRAM[0x10000];
			bool mIE, mRunning;
			u32 mInts;
			ins_fn mInstr[0x100];
	};

	template<typename P>
	std::ostream& operator<<(std::ostream& os, const System<P>& self)
	{
		self.print(os);

		return os;
	}

	template<typename P>
	System<P>::System(void)
	{
		memset((void *) this, 0, sizeof(Self));

		mPC = mSP = 0;
		mIE = mRunning = false;
		mInts = 0;

		mInstr[0x00] = &Self::ins_nop;
		mInstr[0x01] = &Self::ins_halt;
		mInstr[0x02] = &Self::ins_ei;
		mInstr[0x03] = &Self::ins_di;
		mInstr[0x04] = &Self::ins_int;
		mInstr[0x05] = &Self::ins_rst;
		addIns(0x08, 4, &Self::ins_ld16_v);
		addIns(0x0C, 12, &Self::ins_invalid);
		addIns(0x18, 8, &Self::ins_ld8_v);
		addIns(0x20, 8, &Self::ins_math_v);
		addIns(0x28, 8, &Self::ins_c16);
		addIns(0x30, 8, &Self::ins_stack);
		addIns(0x38, 8, &Self::ins_ret);
		addIns(0x40, 16, &Self::ins_ld16);
		addIns(0x50, 16, &Self::ins_io);
		addIns(0x60, 16, &Self::ins_b);
		addIns(0x70, 16, &Self::ins_c8);
		addIns(0x80, 64, &Self::ins_math8);
		addIns(0xC0, 64, &Self::ins_ld8);
	}

	template<typename P>
	void System<P>::start(void)
	{
		P::start();

		mRunning = true;
	}

	template<typename P>
	void System<P>::stop(void)
	{
		P::stop();
	}

	template<typename P>
	void System<P>::addIns(u8 i, u8 n, ins_fn f)
	{
		while(n--)
		{
			mInstr[i++] = f;
		}
	}

	template<typename P>
	void System<P>::load(u16 i0, u16 i1, const byte_t *p)
	{
		while(i0 < i1)
		{
			mRAM[i0++] = *p++;
		}
	}

	template<typename P>
	void System<P>::step(void)
	{
		if(!running())
			return;

		if(mIE && mInts)
		{
			push(mPC);
			mIE = false;
			mPC = 0x0008;
		}

		u8 op = next8();

		(this->*mInstr[op])(op);

		mInts |= P::tick();
	}

	template<typename P>
	void System<P>::ins_invalid(u8 op)
	{
		throw std::string("Invalid instruction!");
	}

	template<typename P>
	void System<P>::ins_nop(u8 op)
	{
	}

	template<typename P>
	void System<P>::ins_halt(u8 op)
	{
		mRunning = false;
	}

	template<typename P>
	void System<P>::ins_ei(u8 op)
	{
		mIE = true;
	}

	template<typename P>
	void System<P>::ins_di(u8 op)
	{
		mIE = false;
	}

	template<typename P>
	void System<P>::ins_int(u8 op)
	{
		A() = mInts;
	}

	template<typename P>
	void System<P>::ins_rst(u8 op)
	{
		mInts = 0;
	}

	template<typename P>
	void System<P>::ins_ld16_v(u8 op)
	{
		reg16(op & 0x03) = next16();
	}

	template<typename P>
	void System<P>::ins_ld8_v(u8 op)
	{
		reg8(op & 0x07) = next8();
	}

	template<typename P>
	void System<P>::ins_math_v(u8 op)
	{
		A() = math(op & 0x7, A(), next8());
	}

	template<typename P>
	void System<P>::ins_stack(u8 op)
	{
		if(op & 4)
		{
			reg16(op & 3) = pop();
		}
		else
		{
			push(reg16(op & 3));
		}
	}

	template<typename P>
	void System<P>::ins_ret(u8 op)
	{
		if(flag(op))
		{
			mPC = pop();
		}
	}

	template<typename P>
	void System<P>::ins_b(u8 op)
	{
		u16 a = next16();

		if(flag(op))
		{
			if(op & 8)
			{
				push(mPC);
			}

			mPC = a;
		}
	}

	template<typename P>
	void System<P>::ins_c8(u8 op)
	{
		u8& r{reg8(op & 0x7)};

		if(op & 0x8)
		{
			r = math(0, r, 1);
		}
		else
		{
			r = math(2, r, 1);
		}
	}

	template<typename P>
	void System<P>::ins_c16(u8 op)
	{
		u16& r{reg16(op & 0x3)};

		if(op & 0x4)
		{
			r = math(0, r, 1);
		}
		else
		{
			r = math(2, r, 1);
		}
	}

	template<typename P>
	void System<P>::ins_io(u8 op)
	{
		if(op & 0x8)
		{
			A() = P::in(reg8(op & 0x7));
		}
		else
		{
			P::out(reg8(op & 0x7), A());
		}
	}

	template<typename P>
	void System<P>::ins_ld16(u8 op)
	{
		reg16((op >> 3) & 0x7) = reg16(op & 0x7);
	}

	template<typename P>
	void System<P>::ins_math8(u8 op)
	{
		A() = math((op >> 3) & 7, A(), reg8(op & 7));
	}

	template<typename P>
	void System<P>::ins_ld8(u8 op)
	{
		reg8((op >> 4) & 0xF) = reg8(op & 0xF);
	}

	template<typename P>
	u8 System<P>::math(u8 op, u8 s, u8 v)
	{
		u16 a = s;

		if(op & 0x4)
		{
			switch(op & 0x3)
			{
				case 0:
					a &= v;
					break;

				case 1:
					a |= v;
					break;

				case 2:
					a ^= v;
					break;

				case 3:
					a = math(2, s, v);
					break;
			}
		}
		else
		{
			u16 c = (op & 1) & (F() & FLAG_CARRY);
			u8 f = 0;

			if(op & 0x2)
			{
				a -= v + c;

				if(a > A())
					f |= FLAG_CARRY;
			}
			else
			{
				a += v + c;

				if((a & 0xFF) < A())
					f |= FLAG_CARRY;
			}

			if((a & 0xFF) == 0)
				f |= FLAG_ZERO;

			if(a & 0x80)
				f |= FLAG_NEGATIVE;

			F() = (u8) f;
		}

		return (u8) a;
	}

	template<typename P>
	bool System<P>::flag(u8 op)
	{
		bool f = true;

		if((op & 3) != 3)
		{
			f = (F() >> (op & 3)) & 1;
			if(op & 4) f = !f;
		}

		return f;
	}

	template<typename P>
	void System<P>::print(std::ostream& os) const
	{
		os << "# ============================================================================\n"
		   << "# --- V80 processor:\n"
		   << "# ============================================================================\n"
		   << "# --- " << (running() ? "running" : "halted") << ", next: " << disassemble(mRAM + mPC) << " // 0x" << hex<8>(mRAM[mPC]) << "\n"
		   << "# --- PC=$" << hex<16>(mPC) << ", SP=$" << hex<16>(mSP) << ", A=" << hex<8>(A()) << ", F=" << hex<8>(F())
		   << ", B=" << hex<8>(B()) << ", C=" << hex<8>(C()) << ", D=" << hex<8>(D()) 
		   << ", E=" << hex<8>(E()) << ", H=" << hex<8>(H()) << ", L=" << hex<8>(L()) << "\n"
		   << "# --- Flags: " << ((F() & FLAG_CARRY) ? "C" : "NC") << "," << ((F() & FLAG_ZERO) ? "Z" : "NZ") << "," 
		   << ((F() & FLAG_NEGATIVE) ? "N" : "P") << "\n"
		   << "# ----------------------------------------------------------------------------\n";

		P::print(os);

		os << "# ============================================================================\n";
	}

	template<typename P>
	void System<P>::printMemory(std::ostream& os, u16 i0, u16 i1) const
	{
		os << "#      | 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"
		   << "# -----+-------------------------------------------------\n";
		
		for(uint i = (i0 / 0x10) ; i < (u16) ((i1 + 0x0F) / 0x10) ; ++i)
		{
			os << "# " << hex<16>(i * 0x10) << " | ";
			
			for(uint j = 0 ; j < 0x10 ; ++j)
			{
				u16 p = j + i * 0x10;

				if(p >= i0 && p < i1)
				{
					os << hex<8>(mRAM[i * 0x10 + j]) << " ";
				}
				else
				{
					os << "   ";
				}
			}

			os << "\n";
		}
	}

	namespace periphery
	{
		class Clock : public BasePeriphery
		{
			public:
				bool tick() { ++mCounter; return false; }
				u8 in() { return mCounter; }
				std::string status() const { return stringify("Clock - tick ", (int) mCounter); }
			private:
				u8 mCounter = 0;
		};
	}
}

int main(int argc, char *argv[])
try
{
	using namespace v80;

	System<Periphery<periphery::Clock>> sys;
	byte_t rom[] = {
		0x19, 0x05,       // ld a,5
		0x1A, 0x03,       // ld b,3
		// :loop
		0x81,             // add a
		0x72,             // dec b
		0x65, 0x04, 0x00, // jp nz,:loop
		0x01              // halt
	};

	sys.load(0, sizeof(rom), rom);

	sys.start();

	while(sys.running())
	{
		std::cout << sys << std::endl;

		sys.step();
	}

	sys.print(std::cout);
	sys.printMemory(std::cout, 0, 0x100);
	std::cout << std::endl;

	return 0;
}
catch(const std::string& e)
{
	std::cerr << "Error: " << e << std::endl;
}

