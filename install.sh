#!/bin/sh

make 

if [ "$?" -eq "0" ]; then
	echo "make fininshed ok";
else
	echo "make failed";
	exit 1;
fi

cp -f hash_shield_main /usr/local/bin/

cp -f hash-shield-startup.service /etc/systemd/system/

touch /etc/hs_hash_storage

systemctl enable hash-shield-startup.service

if [ "$?" -eq "0" ]; then
	echo "enabled service successfully";
else
	echo "failed to enable service";
	exit 1;
fi

echo "installation completed successfully";
