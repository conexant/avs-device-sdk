//TODO: License?

#include <ESP/SynaESPDataProvider.h>
#include <AIP/ESPData.h>

#include <stdio.h>
#include <unistd.h>
#include <cstdint>

namespace alexaClientSDK {
namespace esp {

using ESPData = alexaClientSDK::capabilityAgents::aip::ESPData;

SynaESPDataProvider::SynaESPDataProvider() : m_enabled{true} {
}

capabilityAgents::aip::ESPData SynaESPDataProvider::getESPData() {
    fprintf(stdout, "%s\n", "getESPData()");
    if(m_enabled) {
        // Get the ESP values from the device
        char buffer[128];
        int aIndex = 0;

        FILE* pipe = popen("sudo ./host_demo.exe -u --ESP 2 2>/dev/null", "r");
        try {
            if (!pipe) 
                throw 1;

            fgets(buffer, 128, pipe);
        } catch (...) {
            pclose(pipe);
            //TODO: Use proper log
            fprintf(stderr, "getESPData failed\n");
            return ESPData::EMPTY_ESP_DATA;
        }
        pclose(pipe);
        fprintf(stdout, "%s\n", buffer);

        std::string estr(buffer);
        aIndex = estr.find_first_of(',');
        return capabilityAgents::aip::ESPData{ estr.substr(0,aIndex), estr.substr(aIndex+1, estr.length()-aIndex-2)};
}
    return ESPData::EMPTY_ESP_DATA;
}

bool SynaESPDataProvider::isEnabled() const {
    return m_enabled;
}

void SynaESPDataProvider::disable() {
    m_enabled = false;
}

void SynaESPDataProvider::enable() {
    m_enabled = true;
}
}  // namespace esp
}  // namespace alexaClientSDK
