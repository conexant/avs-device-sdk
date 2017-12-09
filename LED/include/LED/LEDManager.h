#ifndef ALEXA_CLIENT_SDK_TLED_H
#define ALEXA_CLIENT_SDK_TLED_H

#include "LEDControl.h"
#include <AVSCommon/SDKInterfaces/DialogUXStateObserverInterface.h>

namespace alexaClientSDK {
namespace led {

class LEDManager : public avsCommon::sdkInterfaces::DialogUXStateObserverInterface {
public:
	LEDManager();
	~LEDManager();
	void onDialogUXStateChanged(avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState state) override;
private:
	LEDControl* ledC;
};
}}
#endif