import sys
import json
import commentjson

if len(sys.argv) != 8:
	print 'Missing or too many arguments'
	sys.exit(1)

#Inputs
configPath = sys.argv[1]
clientId = sys.argv[2]
clientSecret = sys.argv[3]
productId = sys.argv[4]
deviceSerialNumber = sys.argv[5]
refreshToken = sys.argv[6]
locale = sys.argv[7]

#Default values
alertsDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/alerts.db"
settingsDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/settings.db"
certifiedSenderDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/certifiedSender.db"

with open(configPath,'r') as configFile:
    configData = commentjson.load(configFile)
    if not configData.has_key('authDelegate'):
        print 'The config file "' + \
                configPath + \
                '" is missing the field "authDelegate".'
        sys.exit(1)

    configData['authDelegate']['clientId'] = clientId
    configData['authDelegate']['clientSecret'] = clientSecret
    configData['authDelegate']['productId'] = productId
    configData['authDelegate']['deviceSerialNumber'] = deviceSerialNumber
    configData['authDelegate']['refreshToken'] = refreshToken
    configData['settings']['defaultAVSClientSettings']['locale'] = locale
    configData['settings']['databaseFilePath'] = settingsDatabaseFilePath
    configData['alertsCapabilityAgent']['databaseFilePath'] = alertsDatabaseFilePath
    configData['certifiedSender']['databaseFilePath'] = certifiedSenderDatabaseFilePath

with open(configPath,'w') as configFile:
    commentjson.dump(configData, configFile, indent=4, separators=(',',':'))