---
title: DuoBLE
template: reference.hbs
columns: three
order: 1
---

DuoBLE Library
==========

## DuoBLE Library

This library currently supports features for a Bluetooth Low Energy Peripheral that acts as a Server.

### DuoBLE.begin()

Initialize the stack and configure the database of services being offered. Usual ordering:
1. Initialize all services and add each (via [`DuoBLE.addService()`](#duoble-addservice-)).
2. `DuoBLE.begin()`
3. If a Peripheral:
  1. Configure advertising
  2. [`DuoBLE.startAdvertising()`](duoble-startadvertising-)


```cpp
// EXAMPLE USAGE
// Disable WiFi (for now)
SYSTEM_MODE(MANUAL);

void setup() {

  // Configure and add services before DuoBLE.begin()
  DIS.addDeviceInfo("Acme", "M1", "Ser123", "HRev3", "Firm-4", "Soft5");
  DuoBLE.addService(...);
  ...
  DuoBLE.begin();
}
```  

### DuoBLE.end()

De-initialize the Bluetooth Stack.  This is not needed in most applications.

```cpp
// EXAMPLE USAGE
DuoBLE.end();
```

### DuoBLE.setAdvertisementParameters()

Set the advertising parameters:
  * `adv_int_min` the number of 1.25ms intervals between advertisements (7.5ms-4000ms === 6-3200; Defaults to 50ms)
  * `adv_int_max` the number of 1.25ms intervals between advertisements (7.5ms-4000ms === 6-3200; Defaults to 500ms)
  * `adv_type` (BLE_GAP_ADV_TYPE_ADV_IND=>Connectable undirected, BLE_GAP_ADV_TYPE_ADV_DIRECT_IND=>Connectable directed, BLE_GAP_ADV_TYPE_ADV_SCAN_IND=>Scannable undirected, BLE_GAP_ADV_TYPE_ADV_NONCONN_IND=>Non connectable undirected; Defaults to BLE_GAP_ADV_TYPE_ADV_IND)
  *  `direct_address_typ` peer address type (BLE_GAP_ADDR_TYPE_PUBLIC=>Public address,  BLE_GAP_ADDR_TYPE_RANDOM=>Random address; Defaults to BLE_GAP_ADDR_TYPE_PUBLIC)
  * `direct_address` peer's 48-bit address; Defaults to a null address
  * `channel_map` The Advertising channels to use (bit field: BLE_GAP_ADV_CHANNEL_MAP_37, BLE_GAP_ADV_CHANNEL_MAP_38, BLE_GAP_ADV_CHANNEL_MAP_39, BLE_GAP_ADV_CHANNEL_MAP_ALL; Defaults to BLE_GAP_ADV_CHANNEL_MAP_ALL)
  * `filter_policy` How scan & connection requests are filtered (BLE_GAP_ADV_FP_ANY=>Allow scan requests and connect requests from any device., BLE_GAP_ADV_FP_FILTER_SCANREQ=>Filter scan requests with whitelist, BLE_GAP_ADV_FP_FILTER_CONNREQ=>Filter connect requests with whitelist, BLE_GAP_ADV_FP_FILTER_BOTH=>Filter both scan and connect requests with whitelist; Defaults to BLE_GAP_ADV_FP_ANY)

Generally this is only used to adjust the advertising intervals.  (Other features are not fully supported)

The `MS()` macro can be used to convert time (in ms) to the 1.5ms units.

```cpp
// EXAMPLE USAGE

// Set the default values
DuoBLE.setAdvertisementParameters();
```

```cpp
// EXAMPLE USAGE

// Customize the min and max intervals to 1s and 4s respectively
DuoBLE.setAdvertisementParameters(MS(1000), MS(4000));
```

### DuoBLE.startAdvertising()

Start BLE Advertising.  The stack should be configured first. After the stack and advertising parameters are configured, Advertising can be stopped and restarted as needed (See [`DuoBLE.stopAdvertising()`](duoble-stopadvertising-))

Order of use:
1. Declare Service and Characteristic Variables (as globals or dynamically allocated)
2. In Setup:
  1. Configure Services
    1. Initialize Characteristics
    1. Add Characteristics to Services
    1. Add Service to `DueBLE`
  1. `DuoBLE.begin()`
  1. [Optional] Configure advertisement and advertising parameters
  1. `DuoBLE.startAdvertising()`

```cpp
// EXAMPLE USAGE

// Declare Characteristics and Services
BLEService service1(...);
BLECharacteristic char1(...);

void setup() {
    // Initialize Characteristics as necessary
    char1.setValue(...);
    char1.setCallback(...);
    ...
    // Add Characteristics to services
    service1.addCharacteristic(char1);
    ...

    // Add Services to BLE Stack
    DuoBLE.addService(service1);
    ...

    // Configure Stack
    DuoBLE.begin();

    // Optional: Setup advertising data, parameters, and GAP name

    DuoBLE.startAdvertising();
}  
```

### DuoBLE.stopAdvertising()

Stop advertising. It can be restarted with [`DuoBLE.startAdvertising()`](duoble-startadvertising-).

### DuoBLE.advertisingDataClear()



### DuoBLE.advertisingDataAddFlags()

### DuoBLE.advertisingDataAdd16bitUUIDs()

### DuoBLE.advertisingDataAdd128bitUUID()

### DuoBLE.advertisingDataAddName()

### DuoBLE.advertisingDataAdd16BitServiceData()

### DuoBLE.advertisingDataAdd128BitServiceData()

### DuoBLE.advertisingDataAddManufacturerServiceData()

### DuoBLE.advertisingIBeacon()

### DuoBLE.setPreferredConnectionParameters()

### DuoBLE.disconnectFromCentral()

### DuoBLE.isConnectedToCentral()

### DuoBLE.setName()

### DuoBLE.setAppearance()

### DuoBLE.addService()

## BLEService Objects

### BLEService()

### bleServiceInstance.addCharacteristic()

### bleServiceInstance.uuid()

### bleServiceInstance.setListener()

### bleServiceInstance.setCallback()


## BLECharacteristic Objects

### BLECharacteristic()

### bleCharacteristicInstance.setValue()

### bleCharacteristicInstance.setValue()

### bleCharacteristicInstance.getValue()

### bleCharacteristicInstance.size()

### bleCharacteristicInstance.setListener()

### bleCharacteristicInstance.setCallback()

### bleCharacteristicInstance.sendIndicate()

### bleCharacteristicInstance.sendNotify()


## BLEUuid


## Device Information Service (DIS)

### DIS.addDeviceInfo()

## Heart Rate Service
