#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>

#ifdef PACKET_ENCRYPTION
#include <sodium.h>
#endif

#include "dvlnet/abstract_net.h"
#include "utils/stubs.h"

namespace devilution {
namespace net {

enum packet_type : uint8_t {
	// clang-format off
	PT_MESSAGE      = 0x01,
	PT_TURN         = 0x02,
	PT_JOIN_REQUEST = 0x11,
	PT_JOIN_ACCEPT  = 0x12,
	PT_CONNECT      = 0x13,
	PT_DISCONNECT   = 0x14,
	PT_INFO_REQUEST = 0x21,
	PT_INFO_REPLY   = 0x22,
	// clang-format on
};

// Returns NULL for an invalid packet type.
const char *packet_type_to_string(uint8_t packetType);

typedef uint8_t plr_t;
typedef uint32_t cookie_t;
typedef int turn_t;      // change int to something else in devilution code later
typedef int leaveinfo_t; // also change later
#ifdef PACKET_ENCRYPTION
typedef std::array<unsigned char, crypto_secretbox_KEYBYTES> key_t;
#else
// Stub out the key_t defintion as we're not doing any encryption.
using key_t = uint8_t;
#endif

static constexpr plr_t PLR_MASTER = 0xFE;
static constexpr plr_t PLR_BROADCAST = 0xFF;

class packet_exception : public dvlnet_exception {
public:
	const char *what() const throw() override
	{
		return "Incorrect package size";
	}
};

class wrong_packet_type_exception : public packet_exception {
public:
	wrong_packet_type_exception(std::initializer_list<packet_type> expectedTypes, std::uint8_t actual);

	const char *what() const throw() override
	{
		return message_.c_str();
	}

private:
	std::string message_;
};

class packet {
protected:
	packet_type m_type;
	plr_t m_src;
	plr_t m_dest;
	buffer_t m_message;
	turn_t m_turn;
	cookie_t m_cookie;
	plr_t m_newplr;
	buffer_t m_info;
	leaveinfo_t m_leaveinfo;

	const key_t &key;
	bool have_encrypted = false;
	bool have_decrypted = false;
	buffer_t encrypted_buffer;
	buffer_t decrypted_buffer;

public:
	packet(const key_t &k)
	    : key(k) {};

	const buffer_t &Data();

	packet_type Type();
	plr_t Source() const;
	plr_t Destination() const;
	const buffer_t &Message();
	turn_t Turn();
	cookie_t Cookie();
	plr_t NewPlayer();
	const buffer_t &Info();
	leaveinfo_t LeaveInfo();
};

template <class P>
class packet_proc : public packet {
public:
	using packet::packet;
	void process_data();
};

class packet_in : public packet_proc<packet_in> {
public:
	using packet_proc<packet_in>::packet_proc;
	void Create(buffer_t buf);
	void process_element(buffer_t &x);
	template <class T>
	void process_element(T &x);
	void Decrypt(buffer_t buf);
};

class packet_out : public packet_proc<packet_out> {
public:
	using packet_proc<packet_out>::packet_proc;

	template <packet_type t, typename... Args>
	void create(Args... args);

	void process_element(buffer_t &x);
	template <class T>
	void process_element(T &x);
	template <class T>
	static const unsigned char *begin(const T &x);
	template <class T>
	static const unsigned char *end(const T &x);
	static cookie_t GenerateCookie();
	void Encrypt();
};

template <class P>
void packet_proc<P>::process_data()
{
	P &self = static_cast<P &>(*this);
	self.process_element(m_type);
	self.process_element(m_src);
	self.process_element(m_dest);
	switch (m_type) {
	case PT_MESSAGE:
		self.process_element(m_message);
		break;
	case PT_TURN:
		self.process_element(m_turn);
		break;
	case PT_JOIN_REQUEST:
		self.process_element(m_cookie);
		self.process_element(m_info);
		break;
	case PT_JOIN_ACCEPT:
		self.process_element(m_cookie);
		self.process_element(m_newplr);
		self.process_element(m_info);
		break;
	case PT_CONNECT:
		self.process_element(m_newplr);
		self.process_element(m_info);
		break;
	case PT_DISCONNECT:
		self.process_element(m_newplr);
		self.process_element(m_leaveinfo);
		break;
	case PT_INFO_REPLY:
		self.process_element(m_info);
		break;
	case PT_INFO_REQUEST:
		break;
	}
}

inline void packet_in::process_element(buffer_t &x)
{
	x.insert(x.begin(), decrypted_buffer.begin(), decrypted_buffer.end());
	decrypted_buffer.resize(0);
}

template <class T>
void packet_in::process_element(T &x)
{
	if (decrypted_buffer.size() < sizeof(T))
		throw packet_exception();
	std::memcpy(&x, decrypted_buffer.data(), sizeof(T));
	decrypted_buffer.erase(decrypted_buffer.begin(),
	    decrypted_buffer.begin() + sizeof(T));
}

template <>
inline void packet_out::create<PT_INFO_REQUEST>(plr_t s, plr_t d)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_INFO_REQUEST;
	m_src = s;
	m_dest = d;
}

template <>
inline void packet_out::create<PT_INFO_REPLY>(plr_t s, plr_t d, buffer_t i)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_INFO_REPLY;
	m_src = s;
	m_dest = d;
	m_info = std::move(i);
}

template <>
inline void packet_out::create<PT_MESSAGE>(plr_t s, plr_t d, buffer_t m)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_MESSAGE;
	m_src = s;
	m_dest = d;
	m_message = std::move(m);
}

template <>
inline void packet_out::create<PT_TURN>(plr_t s, plr_t d, turn_t u)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_TURN;
	m_src = s;
	m_dest = d;
	m_turn = u;
}

template <>
inline void packet_out::create<PT_JOIN_REQUEST>(plr_t s, plr_t d,
    cookie_t c, buffer_t i)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_JOIN_REQUEST;
	m_src = s;
	m_dest = d;
	m_cookie = c;
	m_info = i;
}

template <>
inline void packet_out::create<PT_JOIN_ACCEPT>(plr_t s, plr_t d, cookie_t c,
    plr_t n, buffer_t i)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_JOIN_ACCEPT;
	m_src = s;
	m_dest = d;
	m_cookie = c;
	m_newplr = n;
	m_info = i;
}

template <>
inline void packet_out::create<PT_CONNECT>(plr_t s, plr_t d, plr_t n, buffer_t i)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_CONNECT;
	m_src = s;
	m_dest = d;
	m_newplr = n;
	m_info = i;
}

template <>
inline void packet_out::create<PT_CONNECT>(plr_t s, plr_t d, plr_t n)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_CONNECT;
	m_src = s;
	m_dest = d;
	m_newplr = n;
}

template <>
inline void packet_out::create<PT_DISCONNECT>(plr_t s, plr_t d, plr_t n,
    leaveinfo_t l)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	have_decrypted = true;
	m_type = PT_DISCONNECT;
	m_src = s;
	m_dest = d;
	m_newplr = n;
	m_leaveinfo = l;
}

inline void packet_out::process_element(buffer_t &x)
{
	decrypted_buffer.insert(decrypted_buffer.end(), x.begin(), x.end());
}

template <class T>
void packet_out::process_element(T &x)
{
	decrypted_buffer.insert(decrypted_buffer.end(), begin(x), end(x));
}

template <class T>
const unsigned char *packet_out::begin(const T &x)
{
	return reinterpret_cast<const unsigned char *>(&x);
}

template <class T>
const unsigned char *packet_out::end(const T &x)
{
	return reinterpret_cast<const unsigned char *>(&x) + sizeof(T);
}

class packet_factory {
	key_t key = {};
	bool secure;

public:
	static constexpr unsigned short max_packet_size = 0xFFFF;

	packet_factory();
	packet_factory(std::string pw);
	std::unique_ptr<packet> make_packet(buffer_t buf);
	template <packet_type t, typename... Args>
	std::unique_ptr<packet> make_packet(Args... args);
};

inline std::unique_ptr<packet> packet_factory::make_packet(buffer_t buf)
{
	auto ret = std::make_unique<packet_in>(key);
#ifndef PACKET_ENCRYPTION
	ret->Create(std::move(buf));
#else
	if (!secure)
		ret->Create(std::move(buf));
	else
		ret->Decrypt(std::move(buf));
#endif
	ret->process_data();
	return ret;
}

template <packet_type t, typename... Args>
std::unique_ptr<packet> packet_factory::make_packet(Args... args)
{
	auto ret = std::make_unique<packet_out>(key);
	ret->create<t>(args...);
	ret->process_data();
#ifdef PACKET_ENCRYPTION
	if (secure)
		ret->Encrypt();
#endif
	return ret;
}

} // namespace net
} // namespace devilution
