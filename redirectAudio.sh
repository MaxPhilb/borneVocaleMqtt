pacmd load-module module-remap-source master=alsa_input.usb-C-Media_Electronics_Inc._USB_Audio_Device-00.mono-fallback source_name=mono channels=1 channel_map=left rate=16000
pacmd set-default-source mono
pactl list sources short
