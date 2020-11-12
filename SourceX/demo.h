/**
 * @file demo.h
 *
 * Recording / playing demo features
 */
#ifndef __DEMO_H__
#define __DEMO_H__

DEVILUTION_BEGIN_NAMESPACE

extern std::deque<demoMsg> demo_message_queue;
extern std::deque<demoMsg> demo_message_queue_tmp;

extern bool recordDemo;
extern bool demoMode;
extern bool timedemo;
extern bool setDemo;
extern bool processedDemoMsg;
extern bool saveDemoCharCopy;
extern bool loadDemoCharCopy;

void SaveDemo();
void LoadDemo();

DEVILUTION_END_NAMESPACE

#endif /* __DEMO_H__ */
