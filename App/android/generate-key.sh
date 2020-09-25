#!/bin/sh

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

rm -f "${DIR}/key.jks"
rm -f "${DIR}/key.properties"

keytool -genkey -v -keystore "${DIR}/key.jks" -storetype JKS -keyalg RSA -keysize 2048 -validity 10000 -alias key -storepass 123456 -keypass 123456 -dname "CN=Mistic, O=StrangePlanet, L=Lyon, S=ARA, C=FR"

echo "storePassword=123456
keyPassword=123456
keyAlias=key
storeFile=..\\key.jks" > "${DIR}/key.properties"
