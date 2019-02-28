//TODO: License?

#ifndef ALEXA_CLIENT_SDK_ESP_INCLUDE_ESP_SYNAESPDATAPROVIDER_H_
#define ALEXA_CLIENT_SDK_ESP_INCLUDE_ESP_SYNAESPDATAPROVIDER_H_

#include <string>

#include <AIP/ESPData.h>
#include <ESP/ESPDataProviderInterface.h>

namespace alexaClientSDK {
namespace esp {

/**
 * 
 */
class SynaESPDataProvider : public ESPDataProviderInterface{
public:
    /**
     * SynaESPDataProvider Constructor.
     */
    SynaESPDataProvider();

    /// @name Overridden ESPDataProviderInterface methods.
    /// @{
    capabilityAgents::aip::ESPData getESPData() override;
    bool isEnabled() const override;
    void disable() override;
    void enable() override;
    /// @}



private:
    bool m_enabled;
};

}  // namespace esp
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_ESP_INCLUDE_ESP_SYNAESPDATAPROVIDER_H_
