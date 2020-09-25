del "%~dp0\key.jks"
del "%~dp0\key.properties"

"C:\Program Files\Android\Android Studio\jre\bin\keytool" -genkey -v -keystore "%~dp0\key.jks" -storetype JKS -keyalg RSA -keysize 2048 -validity 10000 -alias key -storepass 123456 -keypass 123456 -dname "CN=Mistic, O=StrangePlanet, L=Lyon, S=ARA, C=FR"

(
echo storePassword=123456
echo keyPassword=123456
echo keyAlias=key
echo storeFile=..\\key.jks
) > "%~dp0\key.properties"
