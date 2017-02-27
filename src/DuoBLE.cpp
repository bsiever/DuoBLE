/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#include"DuoBLE.h"
#include"gap.h"
#define MIN(a,b) ((a<b)?(a):(b))


// Enable/disable println debugging
#if 0
#define DEBUG_PRINTLN(x)    Serial.println(x);
#define DEBUG_PRINT(x)      Serial.print(x);
#define DEBUG_PRINTLN2(x,y)  Serial.println(x,y);
#define DEBUG_PRINT2(x,y)    Serial.print(x,y);
#else
#define DEBUG_PRINTLN(x)     ;
#define DEBUG_PRINT(x)       ;
#define DEBUG_PRINTLN2(x,y)  ;
#define DEBUG_PRINT2(x,y)    ;
#endif

// **** Forward declarations
static uint16_t bleGattServerRead(uint16_t handle, uint8_t * buffer, uint16_t buffer_size);
static int bleGattServerWrite(uint16_t handle, uint8_t *buffer, uint16_t buffer_size);
static void bleConnected(BLEStatus_t status, uint16_t handle);
static void bleDisconnected(uint16_t handle);

// Global Variables
bd_addr_t null_address = {0,0,0,0,0,0};

DuoBLE_& DuoBLE = DuoBLE_::getInstance(); // Singleton object


// GAP Services
BLEService gap("1800"); // 1800; 2A01; 2A02; 2A03; 2A04
BLECharacteristic gapAppearance("2A01", ATT_PROPERTY_READ, 2, 2);
// REVIEW : Name is read only
BLECharacteristic gapName("2A00", ATT_PROPERTY_READ);
BLECharacteristic gapPreferredConnectionParameters("2A04", ATT_PROPERTY_READ, 8, 8);

// GATT Services
BLEService gatt("1801"); // 1801;
BLECharacteristic gattServiceChanged("2A05",ATT_PROPERTY_INDICATE, 4, 4);

void DuoBLE_::gapGattInit() {
  // Set default values
  gap.addCharacteristic(gapPreferredConnectionParameters);
  //  REVIEW: gap.addCharacteristic(gapReconnectionAddress);
  gap.addCharacteristic(gapAppearance);
  gap.addCharacteristic(gapName);
  addService(gap);

  // Configure GATT
  gatt.addCharacteristic(gattServiceChanged);
  // Assume the entire table has changed:
  uint8_t gattParameters[4] = {0x01,0x00,0xFF,0xFF};
  gattServiceChanged.setValue(gattParameters,4);
  addService(gatt);
  // REVIEW:
  gattServiceChanged.setListener(*this);
}


DuoBLE_::DuoBLE_() : m_serviceChain(NULL) {
  m_advDataLength[ADVERTISEMENT] = 0;
  m_advDataLength[SCAN_RESPONSE] = 0;
  // Set Default GAP Name
  gapName.setValue("NoName", 6);
  // Default Appearance: Unknown
  setAppearance();
  setPreferredConnectionParameters();
  m_centralConnectionHandle = 0;
  gapGattInit();
}

void DuoBLE_::begin() {
  hal_btstack_init();

  // Enable debugging
  hal_btstack_Log_info(true);
  hal_btstack_error_info(true);

  // Setup callback chain
  hal_btstack_setConnectedCallback(bleConnected);
  hal_btstack_setDisconnectedCallback(bleDisconnected);
  hal_btstack_setGattCharsRead(bleGattServerRead);
  hal_btstack_setGattCharsWrite(bleGattServerWrite);

/*  // Configure callbacks
  // Server Callbacks
  gattCharacteristicReadCallback    = NULL;
  gattCharacteristicWrittenCallback = NULL;

  // REVIEW: Unused or Unsupported Callbacks

  // Server
  gattDescriptorReadCallback        = NULL;
  gattDescriptorticWrittenCallback  = NULL;
  gattWriteCCCDCallback             = NULL;

  // Central Callbacks
  bleAdvertismentCallback           = NULL;

  // Client
  gattServiceDiscoveredCallback     = NULL;
  gattCharsDiscoveredCallback       = NULL;
  gattDescriptorsDiscoveredCallback = NULL;
  gattNotifyUpdateCallback          = NULL;
  gattIndicateUpdateCallback        = NULL;
*/

  // Configure Advertising default parameters
  setAdvertisementParameters();
  // Configure the GATT Server's Database
  configAttDb();

  advertisingDataAddFlags(ADVERTISEMENT, BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
}

void DuoBLE_::end() {
  hal_btstack_deInit();
}

void DuoBLE_::setAdvertisementParameters(uint16_t adv_int_min ,
                                         uint16_t adv_int_max,
                                         uint8_t adv_type,
                                         uint8_t direct_address_typ,
                                         bd_addr_t direct_address,
                                         uint8_t channel_map,
                                         uint8_t filter_policy) {
  assert(adv_int_min>=MS(7.5) && adv_int_min<=MS(4000), "Invalid Min Adv. Int");
  assert(adv_int_max>=MS(7.5) && adv_int_max<=MS(4000) && adv_int_min <= adv_int_max, "Invalid Min Adv. Int");
  assert( (adv_type!=BLE_GAP_ADV_TYPE_ADV_SCAN_IND && adv_type!=BLE_GAP_ADV_TYPE_ADV_NONCONN_IND) || (adv_int_min>=MS(100)), "Adv. must be > 100ms");
  hal_btstack_setAdvertisementParams(adv_int_min, adv_int_max, adv_type, direct_address_typ, direct_address, channel_map, filter_policy);
}

void DuoBLE_::startAdvertising() {
  // Add the advertising data and scan response data
  if(m_advDataLength[ADVERTISEMENT] > 0) {
    hal_btstack_setAdvertisementData(m_advDataLength[ADVERTISEMENT], m_advData[ADVERTISEMENT]);
  }
  if(m_advDataLength[SCAN_RESPONSE] > 0) {
    hal_btstack_setScanResponseData(m_advDataLength[SCAN_RESPONSE], m_advData[SCAN_RESPONSE]);
  }
  hal_btstack_startAdvertising();
}

void DuoBLE_::stopAdvertising() {
  hal_btstack_stopAdvertising();
}

void DuoBLE_::advertisingDataClear(adv_packet_t type) {
  m_advDataLength[type] = 0;
}

void DuoBLE_::advertisingDataAddFlags(adv_packet_t type, uint8_t flags) {
  int startIdx = m_advDataLength[type];
  assert(startIdx+3<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = 2;
  m_advData[type][startIdx+1] = BLE_GAP_AD_TYPE_FLAGS;
  m_advData[type][startIdx+2] = flags;
  m_advDataLength[type]+=3;
}

void DuoBLE_::advertisingDataSetFlags(uint8_t flags) {
  m_advData[ADVERTISEMENT][2] = flags;
}

void DuoBLE_::advertisingDataAdd16bitUUIDs(adv_packet_t type, uint16_t ids[], uint8_t number, uint8_t kind) {
  int startIdx = m_advDataLength[type];
  int totalBytes = 2+2*number;  // Length Byte + Type byte + data (2 bytes each)
  assert(startIdx+totalBytes<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = totalBytes-1;  // Don't count this byte itself
  m_advData[type][startIdx+1] = kind;  // Type
  for(int i=0;i<number;i++) {
      m_advData[type][startIdx+2+2*i] = ids[i] & 0xFF;
      m_advData[type][startIdx+2+2*i+1] = ids[i]>>8 & 0xFF;
  }
  m_advDataLength[type]+=totalBytes;
}

void DuoBLE_::advertisingDataAdd128bitUUID(adv_packet_t type, UUID& id, uint8_t kind) {
  assert(id.size()==16, "Requires 128bit UUID");
  int startIdx = m_advDataLength[type];
  int totalBytes = 2+16;  // Length Byte + Type byte + data (2 bytes each)
  assert(startIdx+totalBytes<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = totalBytes-1;  // Don't count this byte itself
  m_advData[type][startIdx+1] = kind;  // Type
  // Little Endian Order
  for(int i=0;i<16;i++) {
    m_advData[type][startIdx+2+i] = id.bytes128()[15-i];
  }
  m_advDataLength[type]+=totalBytes;
}

void DuoBLE_::advertisingDataAddName(adv_packet_t type, const char *name, uint8_t kind) {
  int length = strlen(name);
  int startIdx = m_advDataLength[type];
  assert(startIdx+2+length<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = length+1;
  m_advData[type][startIdx+1] = kind;
  memcpy((m_advData[type]+startIdx+2), name, length);
  m_advDataLength[type]+=2+length;
}

void DuoBLE_::advertisingDataAdd16BitServiceData(adv_packet_t type,  uint16_t id,  uint8_t data[], uint8_t size) {
  int startIdx = m_advDataLength[type];
  int totalBytes = 2+2+size;  // Length Byte + Type byte + UUID size + data Size
  assert(startIdx+totalBytes<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = totalBytes-1;  // Don't count this byte itself
  m_advData[type][startIdx+1] = BLE_GAP_AD_TYPE_SERVICE_DATA;  // Type
  // Little Endian Order
  m_advData[type][startIdx+2] = (id>>0) & 0xFF;  // UUID
  m_advData[type][startIdx+3] = (id>>8) & 0xFF; // UUID
  memcpy((m_advData[type]+startIdx+4), data, size);  // Data
  m_advDataLength[type]+=totalBytes;
}

void DuoBLE_::advertisingDataAdd128BitServiceData(adv_packet_t type,  UUID& id,  uint8_t data[], uint8_t size) {
  assert(id.size()==16, "Requires 128bit UUID");
  int startIdx = m_advDataLength[type];
  int totalBytes = 2+16+size;
  assert(startIdx+totalBytes<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = totalBytes-1; // Omit this byte itself
  m_advData[type][startIdx+1] = BLE_GAP_AD_TYPE_SERVICE_DATA_128BIT_UUID;
  // Little Endian Order
  for(int i=0;i<16;i++) {
    m_advData[type][startIdx+2+i] = id.bytes128()[15-i];
  }
  memcpy((m_advData[type]+startIdx+18), data, size);
  m_advDataLength[type]+=totalBytes;
}

void DuoBLE_::advertisingDataAddManufacturerServiceData(adv_packet_t type,  uint16_t manufacturer,  uint8_t data[], uint8_t size) {
  int startIdx = m_advDataLength[type];
  int totalBytes = 2+2+size;  // Length Byte + Type byte + UUID size + data Size
  assert(startIdx+totalBytes<31, "Data Won't Fit in Adv. Packet");
  m_advData[type][startIdx] = totalBytes-1;  // Don't count this byte itself
  m_advData[type][startIdx+1] = BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA;  // Type
  // Little Endian Order
  m_advData[type][startIdx+2] = manufacturer & 0xFF;  // UUID
  m_advData[type][startIdx+3] = (manufacturer>>8) & 0xFF; // UUID
  // REVIEW: User should understand byte order (verbatim)
  memcpy((m_advData[type]+startIdx+4), data, size);  // Data
  m_advDataLength[type]+=totalBytes;
}

void DuoBLE_::advertisingIBeacon(UUID& beaconID, uint16_t major, uint16_t minor) {
  // REVIEW: Test w/ a beacon finder
  int type = ADVERTISEMENT;
  int startIdx = m_advDataLength[type];
  assert(startIdx==0, "Adv. Packet must only be used for Beacon");
  assert(beaconID.m_size == 16, "Beacons must include a 128-bit uuid");

  m_advData[type][0] = 0x1A; // Size: 26 bytes long
  m_advData[type][1] = 0xFF; // Type: Manu specific data

  // Manufacturer data: Per iBeacon Format

  // Apple's Company ID
  m_advData[type][2] = 0x4c; // Company Code Byte 1:  Apple Inc.
  m_advData[type][3] = 0x00; // Company Code Byte 2:  Apple Inc.

  // iBeacon Specific format:
  m_advData[type][4] = 0x02; // iBeacon Type
  m_advData[type][5] = 0x15; // iBeacon Data Length (21 bytes)

  // UUID
  for(int i=0;i<16;i++) {
    m_advData[type][i+6] = beaconID.bytes[i]; // Manu specific data
  }
  // Major number
  m_advData[type][22] = major>>8;
  m_advData[type][23] = major & 0xFF;
  // minor number
  m_advData[type][24] = minor>>8;
  m_advData[type][25] = minor & 0xFF;

  // REVIEW: TX Power (should be calibrated...)
  m_advData[type][26] = 0xc8;
  m_advDataLength[type] = 27;
}

// Configure the ATT database.
void DuoBLE_::configAttDb() {
  // Add service and all characteristics
  for(BLEService *service = m_serviceChain; service; service = service->m_nextService) {
    DEBUG_PRINT("Adding: ");
    DEBUG_PRINTLN(service->m_uuid.str());

    if(service->m_uuid.size() == 2) {
      hal_btstack_addServiceUUID16bits(service->m_uuid.value16());
    } else {
      hal_btstack_addServiceUUID128bits(service->m_uuid.bytes128());
    }
    for(BLECharacteristic *characteristic = service->m_characteristicChain; characteristic; characteristic = characteristic->m_next) {
      if(characteristic->m_uuid.size() == 2) {
        characteristic->m_handle = hal_btstack_addCharsDynamicUUID16bits(characteristic->m_uuid.value16(), characteristic->m_properties, characteristic->m_value, characteristic->m_maxSize);
      } else {
        characteristic->m_handle = hal_btstack_addCharsDynamicUUID128bits(characteristic->m_uuid.bytes128(), characteristic->m_properties, characteristic->m_value, characteristic->m_maxSize);
      }
    }
  }
}

void DuoBLE_::setName(const char *name) {
  hal_btstack_setLocalName(name);
  gapName.setValue(name, MIN(strlen(name), MAX_ATT_SZ));
}

void DuoBLE_::setAppearance(uint16_t appearance) {
  uint8_t bytes[2];
  bytes[0] = appearance & 0xFF;
  bytes[1] = appearance >> 8;
  gapAppearance.setValue(bytes, 2);
}

uint16_t DuoBLE_::gattServerRead(uint16_t value_handle, uint8_t * buffer, uint16_t buffer_size) {
  DEBUG_PRINTLN("Read Intercept: ");
  // Avoid double callback (first for length and second for value)
  static unsigned lastRead = 0;
  static unsigned lastHandle = -1;
  const int delta = 6;  // 6ms is min time between connections
  unsigned now = millis();
  DEBUG_PRINTLN(now);

  // Dispatch
  for(BLEService *service = m_serviceChain; service; service = service->m_nextService) {
    for(BLECharacteristic *characteristic = service->m_characteristicChain; characteristic; characteristic = characteristic->m_next) {

      DEBUG_PRINT("Checking Service: ");
      DEBUG_PRINT(service->m_uuid.str());
      DEBUG_PRINT(" Characteristic: ");
      DEBUG_PRINT(characteristic->m_uuid.str());
      DEBUG_PRINTLN(")");


      // If handle matches and readable
      if(characteristic->m_handle == value_handle) {
        DEBUG_PRINT("Read Match (Service: ");
        DEBUG_PRINT(service->m_uuid.str());
        DEBUG_PRINT(" Characteristic: ");
        DEBUG_PRINT(characteristic->m_uuid.str());
        DEBUG_PRINTLN(")");
        // If enough time has passed to be a second call or it's a different handle
        if(now-lastRead > delta || value_handle!=lastHandle) {
          if(characteristic->m_callbackFunction)
            (characteristic->m_callbackFunction)(BLERecipient(value_handle), PREREAD);
            if(characteristic->m_listenerObject)
            characteristic->m_listenerObject->preRead(BLERecipient(value_handle));
        }
          // Do the read
        // TODO: Resolve error with incoming buffer_size
        // memcpy(buffer, characteristic->m_value, MIN(buffer_size, characteristic->m_size));
        memcpy(buffer, characteristic->m_value, characteristic->m_size);

        if(now-lastRead > delta) {
          if(characteristic->m_callbackFunction)
            (characteristic->m_callbackFunction)(BLERecipient(value_handle),POSTREAD);
          if(characteristic->m_listenerObject)
            characteristic->m_listenerObject->postRead(BLERecipient(value_handle));
          }
          lastRead = now;
          lastHandle = value_handle;
          return characteristic->m_size;
      }
      if(characteristic->m_handle+1 == value_handle) {
        if(characteristic->m_properties & (ATT_PROPERTY_NOTIFY | ATT_PROPERTY_INDICATE) ) {
          DEBUG_PRINT("Reading CCCD: ");
          uint8_t data[2];
          data[0] = (characteristic->m_indications << 1) | (characteristic->m_notifications);
          data[1] = 0;
          DEBUG_PRINTLN2(data[0],HEX);
          memcpy(buffer, data, 2);
          lastRead = now;
          lastHandle = value_handle;
          return 2;
        }
      }
    }
  }
}

int DuoBLE_::gattServerWrite(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  DEBUG_PRINTLN("Write Intercept: ");
  // Dispatch
  // Add service and all characteristics
  for(BLEService *service = m_serviceChain; service; service = service->m_nextService) {
    for(BLECharacteristic *characteristic = service->m_characteristicChain; characteristic; characteristic = characteristic->m_next) {
      DEBUG_PRINT("Checking Service: ");
      DEBUG_PRINT(service->m_uuid.str());
      DEBUG_PRINT(" Characteristic: ");
      DEBUG_PRINT(characteristic->m_uuid.str());
      DEBUG_PRINTLN(")");
      // If handle matches and writeable or Notify/Indicate and handle +1
      if(characteristic->m_handle == value_handle) {
        // Do Write
        characteristic->m_size = MIN(size, characteristic->m_maxSize);
        memcpy(characteristic->m_value, buffer, characteristic->m_size);
        // Do Callbacks
        if(characteristic->m_callbackFunction)
          (characteristic->m_callbackFunction)(BLERecipient(characteristic->m_handle), POSTWRITE);
        if(characteristic->m_listenerObject)
          characteristic->m_listenerObject->postWrite(BLERecipient(characteristic->m_handle));
        return 0; // RETURN Success
      }
      if(characteristic->m_handle+1 == value_handle) {
        if(characteristic->m_properties & ATT_PROPERTY_NOTIFY) {
          DEBUG_PRINTLN("Notifiable Char");
          characteristic->m_notifications = buffer[0] & 0x01;
          if(characteristic->m_notifications) {
            DEBUG_PRINTLN("Enabling Notifications");
            if(characteristic->m_callbackFunction)
              (characteristic->m_callbackFunction)(BLERecipient(characteristic->m_handle), NOTIFICATIONS_ENABLED);
            if(characteristic->m_listenerObject) {
              DEBUG_PRINTLN("Sending to listener");
              characteristic->m_listenerObject->notificationsEnabled(BLERecipient(characteristic->m_handle));
            } else {
              DEBUG_PRINTLN("No listener");
            }
          } else {
            DEBUG_PRINTLN("Disabling Notifications");
            if(characteristic->m_callbackFunction)
              (characteristic->m_callbackFunction)(BLERecipient(characteristic->m_handle), NOTIFICATIONS_DISABLED);
            if(characteristic->m_listenerObject)
              characteristic->m_listenerObject->notificationsDisabled(BLERecipient(characteristic->m_handle));
          }
        }
        if(characteristic->m_properties & ATT_PROPERTY_INDICATE) {
            characteristic->m_indications = buffer[0] & 0x02;
            if(characteristic->m_indications) {
              DEBUG_PRINTLN("Enabling Indications");
              if(characteristic->m_callbackFunction)
                (characteristic->m_callbackFunction)(BLERecipient(characteristic->m_handle), INDICATIONS_ENABLED);
              if(characteristic->m_listenerObject) {
                DEBUG_PRINTLN("Sending to listener");
                characteristic->m_listenerObject->indicationsEnabled(BLERecipient(characteristic->m_handle));
              } else {
                DEBUG_PRINTLN("No listener");

              }
            } else {
              DEBUG_PRINTLN("Disabling Indications");
              if(characteristic->m_callbackFunction)
                (characteristic->m_callbackFunction)(BLERecipient(characteristic->m_handle), INDICATIONS_DISABLED);
              if(characteristic->m_listenerObject)
                characteristic->m_listenerObject->indicationsDisabled(BLERecipient(characteristic->m_handle));
            }
        }
        return 0; // RETURN Success
      }
    }
  }
  return 0; // RETURN Success
}

void DuoBLE_::connected(BLEStatus_t status, uint16_t handle) {
  DEBUG_PRINTLN("DUO bleConnected");

  m_centralConnectionHandle = handle;

  // Pass Events to Services
  for(BLEService *service = m_serviceChain; service; service = service->m_nextService) {
    if(service->m_callbackFunction) {
      (service->m_callbackFunction)(handle,CONNECTED);
    }
    if(service->m_listenerObject)
      service->m_listenerObject->connected(handle);
  }
  // Call the static function (last)
  BLE_connected();
}
void DuoBLE_::disconnected(uint16_t handle) {
  DEBUG_PRINTLN("DUO bleDisConnected");

  m_centralConnectionHandle = BLE_INVALID_CONN_HANDLE;

  // Pass Events to Services
  for(BLEService *service = m_serviceChain; service; service = service->m_nextService) {
    // Diable all notifications & indications
    for(BLECharacteristic *characteristic = service->m_characteristicChain; characteristic; characteristic = characteristic->m_next) {
      characteristic->m_indications = false;
      characteristic->m_notifications = false;
    }

    if(service->m_callbackFunction) {
      (service->m_callbackFunction)(handle,DISCONNECTED);
    }
    if(service->m_listenerObject)
      service->m_listenerObject->disconnected(handle);
  }
  // Call the static function (last)
  BLE_disconnected();
}

void DuoBLE_::setPreferredConnectionParameters(uint16_t minInterval,
                                      uint16_t maxInterval,
                                      uint16_t slaveLatency,
                                      uint16_t connectionSupervisionTimeout) {
    uint8_t bytes[8];
    bytes[0] = minInterval & 0xFF;
    bytes[1] = minInterval >> 8;
    bytes[2] = maxInterval & 0xFF;
    bytes[3] = maxInterval >> 8;
    bytes[4] = slaveLatency & 0xFF;
    bytes[5] = slaveLatency >> 8;
    bytes[6] = connectionSupervisionTimeout & 0xFF;
    bytes[7] = connectionSupervisionTimeout >> 8;
    gapPreferredConnectionParameters.setValue(bytes, 8);
}

void DuoBLE_::addService(BLEService &service) {
  // Add to the end of the service chain (to maintain chronological order)
  if(m_serviceChain==NULL) {
    DEBUG_PRINTLN("First is:");
    DEBUG_PRINTLN(service.m_uuid.str());
    m_serviceChain = &service;
  } else {
    DEBUG_PRINTLN("NOT First is:");
    DEBUG_PRINTLN(service.m_uuid.str());
    BLEService *last = m_serviceChain;
    while(last->m_nextService != NULL)
      last = last->m_nextService;
    last->m_nextService = &service;
  }
}

void DuoBLE_::disconnectFromCentral() {
  // REVIEW: Test
  hal_btstack_disconnect(m_centralConnectionHandle);
}

bool DuoBLE_::isConnectedToCentral() {
  // REVIEW: Test
  return m_centralConnectionHandle!=BLE_INVALID_CONN_HANDLE;
}


// C-style callback (just delegate to the object's callbacks)
uint16_t bleGattServerRead(uint16_t handle, uint8_t * buffer, uint16_t buffer_size) {
  return DuoBLE.gattServerRead(handle, buffer, buffer_size);
}
static int bleGattServerWrite(uint16_t handle, uint8_t *buffer, uint16_t buffer_size) {
  return DuoBLE.gattServerWrite(handle, buffer, buffer_size);
}
static void bleConnected(BLEStatus_t status, uint16_t handle) {
  DuoBLE.connected(status, handle);
}
static void bleDisconnected(uint16_t handle) {
  DuoBLE.disconnected(handle);
}
