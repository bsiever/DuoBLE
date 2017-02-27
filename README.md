# DuoBLE

A Bluetooth Low Energy Library for the Red Bear Duo.  Currently only supports peripheral and advertiser roles and server.


# Developer Tips

## To enable HCI packet level debugging:

```C++
hal_btstack_Log_info(true);
hal_btstack_error_info(true);
hal_btstack_enable_packet_info();
DuoBLE.begin();
```

## Event Dispatch
DuoBLE.cpp includes a "debug" macro that can enable debugging messages in event dispatch.

# Known Issues
1. No support for central role yet.
1. No support for client role yet.
1. No support for Security (authentication / authorization / bonding / pairing)
1. No support for authorized writes
1. Characteristic broadcast flag isn't supported
1. Can't add descriptors to Characteristics (btstack_hal doesn't provide access to att_db_util_add_attribute_uuid16())
2. Can't provide different client responses with multiple simultaneous connections (btstack_hal doesn't pass the connection handle to callbacks like gattReadCallback and gattWriteCallback)


# Generating Documentation

## Requirements
1. Doxygen (`brew install doxygen`)
2. Moxygen (`npm install moxygen -g`)

## Running

1. `doxygen`
2  `moxygen --anchors --output duoble.md xml`

## Kudos

- Thanks to [Kam Low](http://sourcey.com/generating-beautiful-markdown-documentation-with-moxygen/) for documentation tips.
- Thanks to [Sandeep Mistry](https://github.com/sandeepmistry)'s [arduin-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) for some inspiration on structure.
