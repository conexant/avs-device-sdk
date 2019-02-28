#include "LED/LEDControl.h"
#include <iostream>
#include <signal.h>

using namespace std;

using namespace  alexaClientSDK::led;
int main () {
	LEDControl ctrl;
/*
	for(int i = 0; i<360; i++) {
		cout << "DOA\n";
		ctrl.DOA(i);
	}
	for(int i = 0; i<1; i++) {
		cout << "volume\n";
		ctrl.volume(70);
	}
	for(int i = 0; i<5; i++) {
		cout << "mute\n";
		ctrl.mute();
	}
	for(int i = 0; i<15; i++) {
		cout << "thinking\n";
		ctrl.thinking();
	}
	for(int i = 0; i<5; i++) {
		cout << "startup\n";
		ctrl.startup();
	}
	for(int i = 0; i<5; i++) {
		cout << "speech\n";
		ctrl.speech();
	}
	for(int i = 0; i<2; i++) {
		cout << "finished\n";
		ctrl.finished();
	}
	for(int i = 0; i<5; i++) {
		cout << "error\n";
		ctrl.error();
	}
	for(int i = 0; i<5; i++) {
		cout << "alarm\n";
		ctrl.alarm();
	}*/
/*	for(int i = 0; i<2; i++) {
		cout << "micMute micUnmute\n";
		ctrl.micMute();
		usleep(200000);
		ctrl.micUnmute();
	}*/
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::IDLE;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::LISTENING;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::THINKING;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::SPEAKING;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::FINISHED;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::ERROR;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::ALARM;
	usleep(10000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::TIMER;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::STARTUP;
	usleep(1000000);
	ctrl.m_stop = 1;
	ctrl.m_state = LEDState::NOTIFICATION;
	usleep(1000000);

	return 0;
}
