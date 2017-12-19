#include "LED/LEDManager.h"


namespace alexaClientSDK {
namespace led {

LEDManager::LEDManager() {
	ledC = new LEDControl;
}

LEDManager::~LEDManager() {
	delete ledC;
}

void LEDManager::onDialogUXStateChanged(
        avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState state) {
	ledC->m_stop = true;
	ledC->m_mutex.lock();

	switch(state){
	case avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState::IDLE:
		ledC->m_state = LEDState::IDLE;
		break;
	case avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState::LISTENING:
		ledC->m_state = LEDState::LISTENING;
		break;
	case avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState::THINKING:
		ledC->m_state = LEDState::THINKING;
		break;
	case avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState::SPEAKING:
		ledC->m_state = LEDState::SPEAKING;
		break;
	case avsCommon::sdkInterfaces::DialogUXStateObserverInterface::DialogUXState::FINISHED:
		ledC->m_state = LEDState::FINISHED;
		break;
	default:
		break;
	}
	
	ledC->m_mutex.unlock();
}

void LEDManager::onSpeakerSettingsChanged(
    const avsCommon::sdkInterfaces::SpeakerManagerObserverInterface::Source& source,
    const avsCommon::sdkInterfaces::SpeakerInterface::Type& type,
    const avsCommon::sdkInterfaces::SpeakerInterface::SpeakerSettings& settings) {
	if(m_oldSettings == settings)
		return;
	m_oldSettings = settings;
	
	ledC->m_stop = true;
	ledC->m_mutex.lock();

	if (settings.mute)
	{ 
		ledC->m_tempState = TempState::MUTED;
	}
	else
	{
		ledC->m_volume = (int)settings.volume;
		ledC->m_tempState = TempState::VOLUME;
	}

	ledC->m_mutex.unlock();
}
}}