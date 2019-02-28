import sys
import json
import commentjson

if len(sys.argv) != 6:
	print 'Missing or too many arguments '
	sys.exit(1)

#Inputs
configPath = sys.argv[1]
clientId = sys.argv[2]
productId = sys.argv[3]
deviceSerialNumber = sys.argv[4]
locale = sys.argv[5]

#Default values
alertsDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/alerts.db"
settingsDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/settings.db"
certifiedSenderDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/certifiedSender.db"
notificationDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/notifications.db"
bluetoothDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/bluetooth.db"
cblAuthDelegateDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/cblAuthDelegate.db"
capabilitiesDelegateDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/capabilitiesDelegate.db"
miscDatabaseFilePath = "/home/pi/sdk-folder/application-necessities/miscDatabase.db"
gstreamerAudioSink = "alsasink"

with open(configPath,'r') as configFile:
    configData = commentjson.load(configFile)
    if not configData.has_key('deviceInfo'):
        print 'The config file "' + \
                configPath + \
                '" is missing the field "deviceInfo".'
        sys.exit(1)

    configData['deviceInfo']['clientId'] = clientId
    #configData['authDelegate']['clientSecret'] = clientSecret
    configData['deviceInfo']['productId'] = productId
    configData['deviceInfo']['deviceSerialNumber'] = deviceSerialNumber
    #configData['deviceInfo']['refreshToken'] = refreshToken
    configData['settings']['defaultAVSClientSettings']['locale'] = locale
    configData['settings']['databaseFilePath'] = settingsDatabaseFilePath
    configData['alertsCapabilityAgent']['databaseFilePath'] = alertsDatabaseFilePath
    configData['certifiedSender']['databaseFilePath'] = certifiedSenderDatabaseFilePath
    configData['notifications']['databaseFilePath'] = notificationDatabaseFilePath
    configData['bluetooth']['databaseFilePath'] = bluetoothDatabaseFilePath
    configData['cblAuthDelegate']['databaseFilePath'] = cblAuthDelegateDatabaseFilePath
    configData['capabilitiesDelegate']['databaseFilePath'] = capabilitiesDelegateDatabaseFilePath
    configData['miscDatabase']['databaseFilePath'] = miscDatabaseFilePath
    configData['gstreamerMediaPlayer']['audioSink'] = gstreamerAudioSink
    
with open(configPath,'w') as configFile:
    commentjson.dump(configData, configFile, indent=4, separators=(',',':'))