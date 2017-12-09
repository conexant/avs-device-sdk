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
		//TODO: log
		break;
	}
	
	ledC->m_mutex.unlock();
}
}}