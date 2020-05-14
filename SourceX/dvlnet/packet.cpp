#include "dvlnet/packet.h"

namespace dvl {
namespace net {

#ifndef NONET
static constexpr bool disable_encryption = false;
#endif

const char *packet_type_to_string(uint8_t packet_type)
{
	switch (packet_type) {
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
	default:
		return NULL;
	}
}

wrong_packet_type_exception::wrong_packet_type_exception(std::initializer_list<packet_type> expected_types, std::uint8_t actual)
{
	message_ = "Expected packet of type ";
	const auto append_packet_type = [this](std::uint8_t t) {
		const char *type_str = packet_type_to_string(t);
		if (type_str != NULL)
			message_.append(type_str);
		else
			message_.append(std::to_string(t));
	};

	constexpr char kJoinTypes[] = " or ";
	for (const packet_type t : expected_types) {
		append_packet_type(t);
		message_.append(kJoinTypes);
	}
	message_.resize(message_.size() - (sizeof(kJoinTypes) - 1));
	message_.append(", got");
	append_packet_type(actual);
}

namespace {

void CheckPacketTypeOneOf(std::initializer_list<packet_type> expected_types, std::uint8_t actual_type) {
	for (std::uint8_t packet_type : expected_types)
		if (actual_type == packet_type) return;
	throw wrong_packet_type_exception(std::move(expected_types), actual_type);
}

} // namespace

const buffer_t &packet::data()
{
	if (!have_decrypted || !have_encrypted)
		ABORT();
	return encrypted_buffer;
}

packet_type packet::type()
{
	if (!have_decrypted)
		ABORT();
	return m_type;
}

plr_t packet::src()
{
	if (!have_decrypted)
		ABORT();
	return m_src;
}

plr_t packet::dest()
{
	if (!have_decrypted)
		ABORT();
	return m_dest;
}

const buffer_t &packet::message()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_MESSAGE}, m_type);
	return m_message;
}

turn_t packet::turn()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_TURN}, m_type);
	return m_turn;
}

cookie_t packet::cookie()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_JOIN_REQUEST, PT_JOIN_ACCEPT}, m_type);
	return m_cookie;
}

plr_t packet::newplr()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_JOIN_ACCEPT, PT_CONNECT, PT_DISCONNECT}, m_type);
	return m_newplr;
}

const buffer_t &packet::info()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_JOIN_REQUEST, PT_JOIN_ACCEPT}, m_type);
	return m_info;
}

leaveinfo_t packet::leaveinfo()
{
	if (!have_decrypted)
		ABORT();
	CheckPacketTypeOneOf({PT_DISCONNECT}, m_type);
	return m_leaveinfo;
}

void packet_in::create(buffer_t buf)
{
	if (have_encrypted || have_decrypted)
		ABORT();
	encrypted_buffer = std::move(buf);
	have_encrypted = true;
}

void packet_in::decrypt()
{
	if (!have_encrypted)
		ABORT();
	if (have_decrypted)
		return;
#ifndef NONET
	if (!disable_encryption) {
		if (encrypted_buffer.size() < crypto_secretbox_NONCEBYTES
				+ crypto_secretbox_MACBYTES
				+ sizeof(packet_type) + 2 * sizeof(plr_t))
			throw packet_exception();
		auto pktlen = (encrypted_buffer.size()
			- crypto_secretbox_NONCEBYTES
			- crypto_secretbox_MACBYTES);
		decrypted_buffer.resize(pktlen);
		if (crypto_secretbox_open_easy(decrypted_buffer.data(),
				encrypted_buffer.data()
					+ crypto_secretbox_NONCEBYTES,
				encrypted_buffer.size()
					- crypto_secretbox_NONCEBYTES,
				encrypted_buffer.data(),
				key.data()))
			throw packet_exception();
	} else
#endif
	{
		if (encrypted_buffer.size() < sizeof(packet_type) + 2 * sizeof(plr_t))
			throw packet_exception();
		decrypted_buffer = encrypted_buffer;
	}

	process_data();

	have_decrypted = true;
}

void packet_out::encrypt()
{
	if (!have_decrypted)
		ABORT();
	if (have_encrypted)
		return;

	process_data();

#ifndef NONET
	if (!disable_encryption) {
		auto len_cleartext = encrypted_buffer.size();
		encrypted_buffer.insert(encrypted_buffer.begin(),
			crypto_secretbox_NONCEBYTES, 0);
		encrypted_buffer.insert(encrypted_buffer.end(),
			crypto_secretbox_MACBYTES, 0);
		randombytes_buf(encrypted_buffer.data(), crypto_secretbox_NONCEBYTES);
		if (crypto_secretbox_easy(encrypted_buffer.data()
					+ crypto_secretbox_NONCEBYTES,
				encrypted_buffer.data()
					+ crypto_secretbox_NONCEBYTES,
				len_cleartext,
				encrypted_buffer.data(),
				key.data()))
			ABORT();
	}
#endif
	have_encrypted = true;
}

packet_factory::packet_factory(std::string pw)
{
#ifndef NONET
	if (sodium_init() < 0)
		ABORT();
	pw.resize(std::min<std::size_t>(pw.size(), crypto_pwhash_argon2id_PASSWD_MAX));
	pw.resize(std::max<std::size_t>(pw.size(), crypto_pwhash_argon2id_PASSWD_MIN), 0);
	std::string salt("devilution-salt 0.2.0");
	salt.resize(crypto_pwhash_argon2id_SALTBYTES, 0);
	if (crypto_pwhash(key.data(), crypto_secretbox_KEYBYTES,
			pw.data(), pw.size(),
			reinterpret_cast<const unsigned char *>(salt.data()),
			crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE,
			crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE,
			crypto_pwhash_ALG_ARGON2ID13))
		ABORT();
#endif
}

} // namespace net
} // namespace dvl
