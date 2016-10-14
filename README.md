# Part HW dari SGU

Under construction

Base paska pimnas, pake ESP8266

Kalo first run ngeupload ke MC, perhatikan dependencies dibawah:

# Dependencies
* board esp8266 di arduino (link menyusul)
* library BH1750 (link menyusul)
* library DHT_sensor_library (link menyusul)
* menyusul

# Skematik Hardware

# Thingspeak

# Ino File
BaudTestESP.ino Uji || AT Command ke ESP8266 dari mc

MegaToThingspeak.ino || mc + ESP8266 terhubung dengan Thingspeak

ESP8266_Thingspeak_BH.ino || Kirim data dari mc + BH1750 ke Thingspeak

ESP8266_Thingspeak_pH.ino || Kirim data dari mc + pH ke Thingspeak

ESP8266_Thingspeak_PH_Light.ino || Kirim data dari mc + pH + BH1750 ke Thingspeak

Thingspeak coba2 :

Channel ID = 170275

id = Smart Hydros

pass = smartgarden
