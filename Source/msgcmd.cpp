#include <new>      // for placement new
#include <stddef.h> // for offsetof
#include <typeinfo> // for typeid

#include "diablo.h"
#include "list.h"

DEVILUTION_BEGIN_NAMESPACE

#define COMMAND_LEN 128

// static float msgcmd_init_cpp_value = 0x7F800000;

struct EXTERNMESSAGE {
	LIST_LINK(EXTERNMESSAGE)
	m_Link;
	char command[COMMAND_LEN];
	void *operator new(size_t n, DWORD extralen, int flags)
	{
		return SMemAlloc(n + extralen, (char *)OBJECT_NAME(EXTERNMESSAGE), SLOG_OBJECT, flags | (1 << 3));
	}
	void operator delete(void *address, DWORD extralen, int flags)
	{
	}
	void operator delete(void *address)
	{
	}
	void *Delete(DWORD flags);
};

void *EXTERNMESSAGE::Delete(DWORD flags)
{
	// BUGFIX: this is already called by m_Link's destructor
	m_Link.Unlink();
	this->~EXTERNMESSAGE();
	if ((flags & 0x1) && this) {
		SMemFree(this, "delete", SLOG_FUNCTION, 0);
	}
	return this;
}

static TList<EXTERNMESSAGE> sgChat_Cmd;

void msgcmd_cmd_cleanup()
{
	sgChat_Cmd.DeleteAll();
}

void msgcmd_send_chat()
{
	DWORD tick;
	struct EXTERNMESSAGE *msg = sgChat_Cmd.Head();

	if (msg) {
		static DWORD sgdwMsgCmdTimer;
		tick = GetTickCount();
		if (tick - sgdwMsgCmdTimer >= 2000) {
			sgdwMsgCmdTimer = tick;
			SNetSendServerChatCommand(msg->command);
			sgChat_Cmd.Remove(msg);
		}
	}
}

BOOL msgcmd_add_server_cmd_W(const char *chat_message)
{
	if (chat_message[0] != '/')
		return FALSE;
	msgcmd_add_server_cmd(chat_message);
	return TRUE;
}

void msgcmd_add_server_cmd(const char *command)
{
	size_t len = strlen(command);
	if (len && ++len <= COMMAND_LEN) {
		struct EXTERNMESSAGE *msg = sgChat_Cmd.Create();
		memcpy(msg->command, command, len);
	}
}

DEVILUTION_END_NAMESPACE
