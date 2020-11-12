/**
 * @file demo.cpp
 *
 * Recording / playing demo features
 */
#include "all.h"
#include "../3rdParty/Storm/Source/storm.h"

DEVILUTION_BEGIN_NAMESPACE

std::deque<demoMsg> demo_message_queue;
std::deque<demoMsg> demo_message_queue_tmp;
bool recordDemo = false;
bool demoMode = false;
bool timedemo = false;
bool setDemo = false;
bool processedDemoMsg = false;
bool saveDemoCharCopy = false;
bool loadDemoCharCopy = false;

class SaveHelper {
	BYTE *tmpbuff;

public:
	SaveHelper(BYTE *b)
	{
		tmpbuff = qolbuff = b;
	}
	~SaveHelper()
	{
		mem_free_dbg(tmpbuff);
	}
};


void SaveDemo()
{
	if (demo_message_queue.empty())
		return;

	DWORD size = demo_message_queue.size();
	DWORD baseLen = size * sizeof(int) * 4 + sizeof(int) * 2;
	DWORD encodedLen = codec_get_encoded_len(baseLen);
	BYTE *SaveBuff = DiabloAllocPtr(encodedLen);
	SaveHelper q(SaveBuff);

	demoMsg first = demo_message_queue.front();
	demoMsg last = demo_message_queue.back();

	SDL_Log("SAVING TICKS BETWEEN %d - %d  : LOGICTICK: %d", first.tick, last.tick, logicTick);

	QOLCopyInts(&logicTick, 1, qolbuff);
	QOLCopyInts(&size, 1, qolbuff);
	SDL_Log("SAVING DEMO SIZE: %d", size);
	for (DWORD i = 0; i < size; i++) {
		demoMsg d = demo_message_queue[i];
		QOLCopyInts(&d.tick, 4, qolbuff);		
	}

	pfile_write_save_file("demo", SaveBuff, baseLen, encodedLen);
	gbValidDemoFile = TRUE;
}

void LoadDemo()
{
	DWORD dwLen;
	BYTE *LoadBuff = pfile_read("demo", &dwLen, TRUE);

	if (LoadBuff != NULL) {
		demo_message_queue.clear();
		demo_message_queue_tmp.clear();
		SaveHelper q(LoadBuff);
		DWORD size;
		QOLCopyInts(qolbuff, 1, &logicTick);
		QOLCopyInts(qolbuff, 1, &size);
		SDL_Log("LOADED DEMO SIZE: %d", size);

		std::ofstream myfile;
		myfile.open("example.txt", std::ios::app);
		myfile << "LOADED SIZE: " << size << "\n";
		for (DWORD i = 0; i < size; i++) {
			demoMsg d;
			QOLCopyInts(qolbuff, 4, &d);
			demo_message_queue.push_back(d);
			demo_message_queue_tmp.push_back(d);
			myfile << i << " " << d.tick << " " << d.message << " " << d.wParam << " " << d.lParam << "\n";
		}
		myfile.close();

		demoMsg first = demo_message_queue.front();
		demoMsg last = demo_message_queue.back();

		SDL_Log("LOADING TICKS BETWEEN %d - %d  : LOGICTICK: %d", first.tick, last.tick, logicTick);
	}
}


DEVILUTION_END_NAMESPACE
