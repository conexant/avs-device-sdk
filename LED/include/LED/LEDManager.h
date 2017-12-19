#ifndef ALEXA_CLIENT_SDK_TLED_H
#define ALEXA_CLIENT_SDK_TLED_H

#include "LEDControl.h"
#include <AVSCommon/SDKInterfaces/DialogUXStateObserverInterface.h>
#include <AVSCommon/SDKInterfaces/SpeakerInterface.h>
#include <AVSCommon/SDKInterfaces/SpeakerManagerObserverInterface.h>

namespace alexaClientSDK {
namespace led {

class LEDManager : 
	public avsCommon::sdkInterfaces::DialogUXStateObserverInterface,
	public avsCommon::sdkInterfaces::SpeakerManagerObserverInterface {
public:
	LEDManager();
	~LEDManager();
	void onDialogUXStateChanged(avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState state) override;
    void onSpeakerSettingsChanged(
	    const avsCommon::sdkInterfaces::SpeakerManagerObserverInterface::Source& source,
	    const avsCommon::sdkInterfaces::SpeakerInterface::Type& type,
	    const avsCommon::sdkInterfaces::SpeakerInterface::SpeakerSettings& settings) override;
private:
	LEDControl* ledC;
	avsCommon::sdkInterfaces::SpeakerInterface::SpeakerSettings m_oldSettings;
};
}}
#endif