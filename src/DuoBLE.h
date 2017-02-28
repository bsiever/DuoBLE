/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#ifndef _DuoBLE_h_
#define _DuoBLE_h_

#include<Arduino.h>

#include"BLEUuid.h"
#include"BLEService.h"
#include"BLECharacteristic.h"

#include"dble_assert.h"

#if !defined(PLATFORM_DUO_PRODUCTION)
#error "DuoBLE.h is only for use with RedBear Duo boards"
#endif

// Simple C Library Client Callbacks
// Potential Callbacks for C-based API
// GAP Events (connection, disconnection)
extern void BLE_connected() __attribute__((weak));
extern void BLE_disconnected() __attribute__((weak));

// *** Convenience Macros
// Convert time to 1.25ms intervals (rounds down)
#define MS(x) (x*4/5)
#define MS10(x) (x/10)


// **** Forward declarations for internal event handlers
uint16_t bleGattServerRead(uint16_t handle, uint8_t * buffer, uint16_t buffer_size);
int bleGattServerWrite(uint16_t handle, uint8_t *buffer, uint16_t buffer_size);
void bleConnected(BLEStatus_t status, uint16_t handle);
void bleDisconnected(uint16_t handle);


#define BLE_INVALID_CONN_HANDLE 0xFFFF

// *** Constatnts
extern bd_addr_t null_address;

// *** TYPES
// ***  Advertisement Packet Designations
enum adv_packet_t { ADVERTISEMENT, SCAN_RESPONSE};

/**
 * DuoBLE_ class interacts with the Bluetooth Stack (btstack)
 * There is a singleton (single instance) of this class (DuoBLE)
 */
class DuoBLE_ : public BLECharacteristicListener {

public:
  /**
   * Access the instance. Don't call! Use the "DuoBLE" instance instead.
   */
  static DuoBLE_& getInstance() {
    static DuoBLE_ theInstance;
    return theInstance;
  }

  /** Initialize the DuoBLE object.
   *
   * Intitialize the library (this should be called before all other methods)
   */
  void begin();

  /** End use of the DuoBLE library.
   *
   * Stop the library (should be called when the library is no longer needed)
   */
  void end();


/***** Peripheral / Advertiser Role Features *************************/


  /**
    * Set the advertisement parameters.
    *
    * @param adv_int_min the number of 1.25ms intervals between advertisements (7.5ms-4000ms === 6-3200)
    * @param adv_int_max the number of 1.25ms intervals between advertisements (7.5ms-4000ms === 6-3200)
    * @param adv_type (BLE_GAP_ADV_TYPE_ADV_IND=>Connectable undirected), BLE_GAP_ADV_TYPE_ADV_DIRECT_IND=>Connectable directed, BLE_GAP_ADV_TYPE_ADV_SCAN_IND=>Scannable undirected, BLE_GAP_ADV_TYPE_ADV_NONCONN_IND=>Non connectable undirected)
    * @param direct_address_typ peer address type (BLE_GAP_ADDR_TYPE_PUBLIC=>Public address,  BLE_GAP_ADDR_TYPE_RANDOM=>Random address)
    * @param direct_address peer's 48-bit address
    * @param channel_map The Advertising channels to use (bit field: BLE_GAP_ADV_CHANNEL_MAP_37, BLE_GAP_ADV_CHANNEL_MAP_38, BLE_GAP_ADV_CHANNEL_MAP_39, BLE_GAP_ADV_CHANNEL_MAP_ALL)
    * @param filter_policy How scan & connection requests are filtered (BLE_GAP_ADV_FP_ANY=>Allow scan requests and connect requests from any device., BLE_GAP_ADV_FP_FILTER_SCANREQ=>Filter scan requests with whitelist, BLE_GAP_ADV_FP_FILTER_CONNREQ=>Filter connect requests with whitelist, BLE_GAP_ADV_FP_FILTER_BOTH=>Filter both scan and connect requests with whitelist)
    */
  void setAdvertisementParameters(uint16_t adv_int_min = MS(50),
                                  uint16_t adv_int_max = MS(500),
                                  uint8_t adv_type = BLE_GAP_ADV_TYPE_ADV_IND,
                                  uint8_t direct_address_typ = BLE_GAP_ADDR_TYPE_PUBLIC,
                                  bd_addr_t direct_address = null_address,
                                  uint8_t channel_map = BLE_GAP_ADV_CHANNEL_MAP_ALL,
                                  uint8_t filter_policy = BLE_GAP_ADV_FP_ANY);


  /**
   * Start Advertising
   */
   void startAdvertising();
   /**
    * Stop Advertising
    */
   void stopAdvertising();


  // Set of functions to add each component (and clear)

   /**
    * Clear an advertising packet (either Advertising or Scan Response)
    *
    * @param type Either ADVERTISEMENT or SCAN_RESPONSE
    */
   void advertisingDataClear(adv_packet_t type);

   /**
    * Set the flags in advertising packets (required to indicate connectable vs. broadcaster only)
    *
    * @param type Either ADVERTISEMENT or SCAN_RESPONSE
    * @param flags A bit field of flags. (BLE_GAP_ADV_FLAG_LE_LIMITED_DISC_MODE=>LE Limited Discoverable Mode,
                                          BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE=>LE General Discoverable Mode.
                                          BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED=>BR/EDR not supported.
                                          BLE_GAP_ADV_FLAG_LE_BR_EDR_CONTROLLER=>Simultaneous LE and BR/EDR, Controller.
                                          BLE_GAP_ADV_FLAG_LE_BR_EDR_HOST=>Simultaneous LE and BR/EDR, Host)
                                          Convenienve values: BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE=>LE Limited Discoverable Mode, BR/EDR not supported.
                                                              BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE=> LE General Discoverable Mode, BR/EDR not supported.

    */
   void advertisingDataSetFlags(uint8_t flags=BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
   void advertisingDataAdd16bitUUIDs(adv_packet_t type, uint16_t ids[], uint8_t number, uint8_t kind = BLE_GAP_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE);
   void advertisingDataAdd128bitUUID(adv_packet_t type, UUID& id, uint8_t kind = BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE);
   void advertisingDataAddName(adv_packet_t type, const char *name, uint8_t kind = BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME);
   void advertisingDataAdd16BitServiceData(adv_packet_t type,  uint16_t id,  uint8_t data[], uint8_t size);
   void advertisingDataAdd128BitServiceData(adv_packet_t type,  UUID& id,  uint8_t data[], uint8_t size);
   void advertisingDataAddManufacturerServiceData(adv_packet_t type,  uint16_t manufacturer,  uint8_t data[], uint8_t size);
   void advertisingIBeacon(UUID& beaconID, uint16_t major, uint16_t minor);

   // TODO
   //   void advertisingDataAddTxPower(adv_packet_t type, uint8_t power);
   //   void advertisingDataAddAppearance(adv_packet_t type, uint8_t appearance);
   //   Check for others of interest


   /* This sets the GAP preferred connection parameters (for peripheral roles)
   */
   // Units on Connection Supervision Timeout (is in 10ms)
   void setPreferredConnectionParameters(uint16_t minInterval = MS(50),
                                         uint16_t maxInterval = MS(4000),
                                         uint16_t slaceLatency = 5,
                                         uint16_t connectionSupervisionTimeout = MS10(6000));


   void disconnectFromCentral();
   bool isConnectedToCentral();

  /***** GAP Settings *************************/


   /**
    * This sets the GAP name (retrieved after a connection)
    */
   void setName(const char *name);

   /**
    * This sets the GAP Appearance (retrieved after a connection)
    */
   void setAppearance(uint16_t appearance = BLE_APPEARANCE_UNKNOWN); // 16-bit ID from http://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.gap.appearance.xml


   void addService(BLEService& service);


private:
  // Default flags are always added
  void advertisingDataAddFlags(adv_packet_t type, uint8_t flags=BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);

  friend uint16_t bleGattServerRead(uint16_t handle, uint8_t * buffer, uint16_t buffer_size);
  friend int bleGattServerWrite(uint16_t handle, uint8_t *buffer, uint16_t buffer_size);
  friend void bleConnected(BLEStatus_t status, uint16_t handle);
  friend void bleDisconnected(uint16_t handle);

  // Helper functions
  void gapGattInit();
  void configAttDb();

  // Callback handlers
  uint16_t gattServerRead(uint16_t handle, uint8_t * buffer, uint16_t buffer_size);
  int gattServerWrite(uint16_t handle, uint8_t *buffer, uint16_t buffer_size);
  void connected(BLEStatus_t status, uint16_t handle);
  void disconnected(uint16_t handle);

  // Constructor
  DuoBLE_();

  // Fields
  uint8_t m_advData[2][31];   // Advertising data
  uint8_t m_advDataLength[2]; // Length
  BLEService *m_serviceChain;

  uint16_t m_centralConnectionHandle;  // BLE_INVALID_CONN_HANDLE when disconnected
};

extern DuoBLE_& DuoBLE; // Singleton object


#endif
