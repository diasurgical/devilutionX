#include "dvlnet/packet.h"

namespace dvl {
namespace net {

static constexpr bool disable_encryption = false;

const buffer_t& packet::data()
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

const buffer_t& packet::message()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_MESSAGE)
		throw packet_exception();
	return m_message;
}

turn_t packet::turn()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_TURN)
		throw packet_exception();
	return m_turn;
}

cookie_t packet::cookie()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_JOIN_REQUEST && m_type != PT_JOIN_ACCEPT)
		throw packet_exception();
	return m_cookie;
}

plr_t packet::newplr()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_JOIN_ACCEPT && m_type != PT_CONNECT
	    && m_type != PT_DISCONNECT)
		throw packet_exception();
	return m_newplr;
}

const buffer_t& packet::info()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_JOIN_REQUEST && m_type != PT_JOIN_ACCEPT)
		throw packet_exception();
	return m_info;
}

leaveinfo_t packet::leaveinfo()
{
	if (!have_decrypted)
		ABORT();
	if (m_type != PT_DISCONNECT)
		throw packet_exception();
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
    if (encrypted_buffer.size() < sizeof(packet_type) + 2*sizeof(plr_t))
        throw packet_exception();
    decrypted_buffer = encrypted_buffer;

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

	have_encrypted = true;
}

packet_factory::packet_factory(std::string pw)
{
}

}  // namespace net
}  // namespace dvl
