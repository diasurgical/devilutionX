#ifdef PACKET_ENCRYPTION
#include <sodium.h>
#else
#include <chrono>
#include <random>
#endif

#include <cassert>

#include "dvlnet/packet.h"

namespace devilution {
namespace net {

#ifdef PACKET_ENCRYPTION

cookie_t packet_out::GenerateCookie()
{
	cookie_t cookie;
	randombytes_buf(reinterpret_cast<unsigned char *>(&cookie),
	    sizeof(cookie_t));
	return cookie;
}

#else

class cookie_generator {
public:
	cookie_generator()
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		generator.seed(seed);
	}

	cookie_t NewCookie()
	{
		return distribution(generator);
	}

private:
	std::default_random_engine generator;
	std::uniform_int_distribution<cookie_t> distribution;
};

cookie_generator CookieGenerator;

cookie_t packet_out::GenerateCookie()
{
	return CookieGenerator.NewCookie();
}

#endif

const char *packet_type_to_string(uint8_t packetType)
{
	switch (packetType) {
	case PT_MESSAGE:
		return "PT_MESSAGE";
	case PT_TURN:
		return "PT_TURN";
	case PT_JOIN_REQUEST:
		return "PT_JOIN_REQUEST";
	case PT_JOIN_ACCEPT:
		return "PT_JOIN_ACCEPT";
	case PT_CONNECT:
		return "PT_CONNECT";
	case PT_DISCONNECT:
		return "PT_DISCONNECT";
	case PT_INFO_REQUEST:
		return "PT_INFO_REQUEST";
	case PT_INFO_REPLY:
		return "PT_INFO_REPLY";
	case PT_ECHO_REQUEST:
		return "PT_ECHO_REQUEST";
	case PT_ECHO_REPLY:
		return "PT_ECHO_REPLY";
	default:
		return nullptr;
	}
}

wrong_packet_type_exception::wrong_packet_type_exception(std::initializer_list<packet_type> expectedTypes, std::uint8_t actual)
{
	message_ = "Expected packet of type ";
	const auto appendPacketType = [this](std::uint8_t t) {
		const char *typeStr = packet_type_to_string(t);
		if (typeStr != nullptr)
			message_.append(typeStr);
		else
			message_.append(std::to_string(t));
	};

	constexpr char KJoinTypes[] = " or ";
	for (const packet_type t : expectedTypes) {
		appendPacketType(t);
		message_.append(KJoinTypes);
	}
	message_.resize(message_.size() - (sizeof(KJoinTypes) - 1));
	message_.append(", got");
	appendPacketType(actual);
}

namespace {

void CheckPacketTypeOneOf(std::initializer_list<packet_type> expectedTypes, std::uint8_t actualType)
{
	for (std::uint8_t packetType : expectedTypes)
		if (actualType == packetType)
			return;
#if DVL_EXCEPTIONS
	throw wrong_packet_type_exception(expectedTypes, actualType);
#else
	app_fatal("wrong packet type");
#endif
}

} // namespace

const buffer_t &packet::Data()
{
	assert(have_encrypted || have_decrypted);
	if (have_encrypted)
		return encrypted_buffer;
	return decrypted_buffer;
}

packet_type packet::Type()
{
	assert(have_decrypted);
	return m_type;
}

plr_t packet::Source() const
{
	assert(have_decrypted);
	return m_src;
}

plr_t packet::Destination() const
{
	assert(have_decrypted);
	return m_dest;
}

const buffer_t &packet::Message()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_MESSAGE }, m_type);
	return m_message;
}

turn_t packet::Turn()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_TURN }, m_type);
	return m_turn;
}

cookie_t packet::Cookie()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_JOIN_REQUEST, PT_JOIN_ACCEPT }, m_type);
	return m_cookie;
}

plr_t packet::NewPlayer()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_JOIN_ACCEPT, PT_CONNECT, PT_DISCONNECT }, m_type);
	return m_newplr;
}

timestamp_t packet::Time()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_ECHO_REQUEST, PT_ECHO_REPLY }, m_type);
	return m_time;
}

const buffer_t &packet::Info()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_JOIN_REQUEST, PT_JOIN_ACCEPT, PT_CONNECT, PT_INFO_REPLY }, m_type);
	return m_info;
}

leaveinfo_t packet::LeaveInfo()
{
	assert(have_decrypted);
	CheckPacketTypeOneOf({ PT_DISCONNECT }, m_type);
	return m_leaveinfo;
}

void packet_in::Create(buffer_t buf)
{
	assert(!have_encrypted && !have_decrypted);
	if (buf.size() < sizeof(packet_type) + 2 * sizeof(plr_t))
#if DVL_EXCEPTIONS
		throw packet_exception();
#else
		app_fatal("invalid packet");
#endif

	decrypted_buffer = std::move(buf);
	have_decrypted = true;

	// TCP server implementation forwards the original data to clients
	// so although we are not decrypting anything,
	// we save a copy in encrypted_buffer anyway
	encrypted_buffer = decrypted_buffer;
	have_encrypted = true;
}

#ifdef PACKET_ENCRYPTION
void packet_in::Decrypt(buffer_t buf)
{
	assert(!have_encrypted && !have_decrypted);
	encrypted_buffer = std::move(buf);
	have_encrypted = true;

	if (encrypted_buffer.size() < crypto_secretbox_NONCEBYTES
	        + crypto_secretbox_MACBYTES
	        + sizeof(packet_type) + 2 * sizeof(plr_t))
		throw packet_exception();
	auto pktlen = (encrypted_buffer.size()
	    - crypto_secretbox_NONCEBYTES
	    - crypto_secretbox_MACBYTES);
	decrypted_buffer.resize(pktlen);
	int status = crypto_secretbox_open_easy(
	    decrypted_buffer.data(),
	    encrypted_buffer.data() + crypto_secretbox_NONCEBYTES,
	    encrypted_buffer.size() - crypto_secretbox_NONCEBYTES,
	    encrypted_buffer.data(),
	    key.data());
	if (status != 0)
		throw packet_exception();

	have_decrypted = true;
}
#endif

#ifdef PACKET_ENCRYPTION
void packet_out::Encrypt()
{
	assert(have_decrypted);

	if (have_encrypted)
		return;

	auto lenCleartext = decrypted_buffer.size();
	encrypted_buffer.insert(encrypted_buffer.begin(),
	    crypto_secretbox_NONCEBYTES, 0);
	encrypted_buffer.insert(encrypted_buffer.end(),
	    crypto_secretbox_MACBYTES + lenCleartext, 0);
	randombytes_buf(encrypted_buffer.data(), crypto_secretbox_NONCEBYTES);
	int status = crypto_secretbox_easy(
	    encrypted_buffer.data() + crypto_secretbox_NONCEBYTES,
	    decrypted_buffer.data(),
	    lenCleartext,
	    encrypted_buffer.data(),
	    key.data());
	if (status != 0)
		ABORT();

	have_encrypted = true;
}
#endif

packet_factory::packet_factory()
{
	secure = false;
}

packet_factory::packet_factory(std::string pw)
{
	secure = false;

#ifdef PACKET_ENCRYPTION
	if (sodium_init() < 0)
		ABORT();
	pw.resize(std::min<std::size_t>(pw.size(), crypto_pwhash_argon2id_PASSWD_MAX));
	pw.resize(std::max<std::size_t>(pw.size(), crypto_pwhash_argon2id_PASSWD_MIN), 0);
	std::string salt("W9bE9dQgVaeybwr2");
	salt.resize(crypto_pwhash_argon2id_SALTBYTES, 0);
	int status = crypto_pwhash(
	    key.data(),
	    crypto_secretbox_KEYBYTES,
	    pw.data(),
	    pw.size(),
	    reinterpret_cast<const unsigned char *>(salt.data()),
	    3 * crypto_pwhash_argon2id_OPSLIMIT_MIN,
	    2 * crypto_pwhash_argon2id_MEMLIMIT_MIN,
	    crypto_pwhash_ALG_ARGON2ID13);
	if (status != 0)
		ABORT();
	secure = true;
#endif
}

} // namespace net
} // namespace devilution
