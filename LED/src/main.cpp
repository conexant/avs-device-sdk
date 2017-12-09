#include "LEDControl.h"
#include <iostream>
#include <signal.h>

using namespace std;

using namespace  alexaClientSDK::tled;
int main () {
	LEDControl ctrl;

	ctrl.init();

/*	for(int i = 0; i<360; i++) {
		cout << "DOA\n";
		ctrl.DOA(i);
	}
	for(int i = 0; i<1; i++) {
		cout << "volume\n";
		ctrl.volume(70);
	}
	for(int i = 0; i<15; i++) {
		cout << "cloudActivity\n";
		ctrl.cloudActivity();
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
	}*/
	for(int i = 0; i<5; i++) {
		cout << "alarm\n";
		ctrl.alarm();
	}
/*	for(int i = 0; i<2; i++) {
		cout << "micMute micUnmute\n";
		ctrl.micMute();
		usleep(200000);
		ctrl.micUnmute();
	}*/

	ctrl.kill();

	return 0;
}