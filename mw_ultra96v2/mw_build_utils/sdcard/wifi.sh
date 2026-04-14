#Add the wi-fi driver module to the kernel:
cp -rf /mnt/wpa_supplicant.conf /etc/.

# Load ATWILC3000 driver
modprobe wilc-sdio

# Bring up interface
ifconfig wlan0 up

# Run wpa_supplicant for secure networking:
wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant.conf -B

#To add DHCP:
udhcpc -i wlan0

#To run iperf3:  (2.0.5 without the "b")
#server mode:
#comment this to simplify production testing:
#iperf3 -s


