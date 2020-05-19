#pragma once

#include <string>
#include <memory>
#include <array>
#include <cstring>
#ifndef NONET
#include <sodium.h>
#endif

#include "dvlnet/abstract_net.h"
#include "stubs.h"

namespace dvl {
namespace net {

enum packet_type : uint8_t {
	PT_MESSAGE = 0x01,
	PT_TURN = 0x02,
	PT_JOIN_REQUEST = 0x11,
	PT_JOIN_ACCEPT = 0x12,
	PT_CONNECT = 0x13,
	PT_DISCONNECT = 0x14,
};

// Returns NULL for an invalid packet type.
const char *packet_type_to_string(uint8_t packet_type);

typedef uint8_t plr_t;
typedef uint32_t cookie_t;
typedef int turn_t;      // change int to something else in devilution code later
typedef int leaveinfo_t; // also change later
#ifndef NONET
typedef std::array<unsigned char, crypto_secretbox_KEYBYTES> key_t;
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
	wrong_packet_type_exception(std::initializer_list<packet_type> expected_types, std::uint8_t actual);

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

	const buffer_t &data();

	packet_type type();
	plr_t src();
	plr_t dest();
	const buffer_t &message();
	turn_t turn();
	cookie_t cookie();
	plr_t newplr();
	const buffer_t &info();
	leaveinfo_t leaveinfo();
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
	void create(buffer_t buf);
	void process_element(buffer_t &x);
	template <class T>
	void process_element(T &x);
	void decrypt();
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
	void encrypt();
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
		break;
	case PT_DISCONNECT:
		self.process_element(m_newplr);
		self.process_element(m_leaveinfo);
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
	encrypted_buffer.insert(encrypted_buffer.end(), x.begin(), x.end());
}

template <class T>
void packet_out::process_element(T &x)
{
	encrypted_buffer.insert(encrypted_buffer.end(), begin(x), end(x));
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

public:
	static constexpr unsigned short max_packet_size = 0xFFFF;

	packet_factory(std::string pw = "");
	std::unique_ptr<packet> make_packet(buffer_t buf);
	template <packet_type t, typename... Args>
	std::unique_ptr<packet> make_packet(Args... args);
};

inline std::unique_ptr<packet> packet_factory::make_packet(buffer_t buf)
{
	std::unique_ptr<packet_in> ret(new packet_in(key));
	ret->create(std::move(buf));
	ret->decrypt();
	return std::unique_ptr<packet>(std::move(ret));
}

template <packet_type t, typename... Args>
std::unique_ptr<packet> packet_factory::make_packet(Args... args)
{
	std::unique_ptr<packet_out> ret(new packet_out(key));
	ret->create<t>(args...);
	ret->encrypt();
	return std::unique_ptr<packet>(std::move(ret));
}

} // namespace net
} // namespace dvl
