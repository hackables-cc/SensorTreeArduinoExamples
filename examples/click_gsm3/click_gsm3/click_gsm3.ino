/**************************************************************
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 * NOTE:
 * Some of the functions may be unavailable for your modem.
 * Just comment them out.
 *
 **************************************************************/

//////////////////////////////////////////////////////////////////////
// Uncomment the code coorrespondent to the Slot that the board is  // 
// fitted on.                                                       //
//////////////////////////////////////////////////////////////////////
/* MIKROBUS 1 */
#define GSM3CLICK_MKBUS_1
#define GSM_RESET   8
#define GSM_POWER   3
#define GSM_STATUS  14
#define SerialAT Serial1

/* MIKROBUS 2 */
//#define GSM3CLICK_MKBUS_2
//#define GSM_RESET   17
//#define GSM_POWER   4
//#define GSM_STATUS  15
//#define SerialAT Serial2

/* MIKROBUS 3 */
//#define GSM3CLICK_MKBUS_3
//#define GSM_RESET   38
//#define GSM_POWER   10
//#define GSM_STATUS  16
//#define SerialAT Serial3

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial


//#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

// Set phone numbers, if you want to test SMS and Calls
//#define SMS_TARGET  "+351918283647"
#define CALL_TARGET "+351918283647"

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

void setup() {
  // initialize serials
  
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // initialize GSM serial communication at 9600 bits per second:
  SerialAT.begin(9600);


  // Set your reset, enable, power pins here
  pinMode(GSM_RESET, OUTPUT);
  pinMode(GSM_POWER, OUTPUT);
  pinMode(GSM_STATUS, INPUT);
   
  // Hard Reset the GSM module
  do
  {
    DBG("Hard Reset modem...");
    digitalWrite(GSM_POWER, HIGH);
    delay(1500);
    digitalWrite(GSM_POWER, LOW);
    delay(1500);    
  }while( digitalRead(GSM_STATUS) != 1 );

  delay(3000);

  // Set GSM module baud rate
  TinyGsmAutoBaud(SerialAT);
}

void loop() {

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    delay(10000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    DBG("Network connected");
  }

//  DBG("Connecting to", apn);
//  if (!modem.gprsConnect(apn, user, pass)) {
//    delay(10000);
//    return;
//  }

//  bool res = modem.isGprsConnected();
//  DBG("GPRS status:", res ? "connected" : "not connected");
//
//  String ccid = modem.getSimCCID();
//  DBG("CCID:", ccid);

  String imei = modem.getIMEI();
  DBG("IMEI:", imei);

//  String cop = modem.getOperator();
//  DBG("Operator:", cop);
//  
//  IPAddress local = modem.localIP();
//  DBG("Local IP:", local);
//
//  int csq = modem.getSignalQuality();
//  DBG("Signal quality:", csq);
//
//  // This is NOT supported on M590
//  int battLevel = modem.getBattPercent();
//  DBG("Battery lavel:", battLevel);
//
//  // This is only supported on SIMxxx series
//  float battVoltage = modem.getBattVoltage() / 1000.0F;
//  DBG("Battery voltage:", battVoltage);
//
//  // This is only supported on SIMxxx series
//  String gsmLoc = modem.getGsmLocation();
//  DBG("GSM location:", gsmLoc);
//
//  // This is only supported on SIMxxx series
//  String gsmTime = modem.getGSMDateTime(DATE_TIME);
//  DBG("GSM Time:", gsmTime);
//  String gsmDate = modem.getGSMDateTime(DATE_DATE);
//  DBG("GSM Date:", gsmDate);
//
//  String ussd_balance = modem.sendUSSD("*#123#");
//  DBG("Balance (USSD):", ussd_balance);
//
////  String ussd_phone_num = modem.sendUSSD("*161#");
////  DBG("Phone number (USSD):", ussd_phone_num);
//
//#if defined(TINY_GSM_MODEM_SIM808)
//  modem.enableGPS();
//  String gps_raw = modem.getGPSraw();
//  modem.disableGPS();
//  DBG("GPS raw data:", gps_raw);
//#endif

#if defined(SMS_TARGET)
  bool ressms = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
  DBG("SMS:", ressms ? "OK" : "fail");

  // This is only supported on SIMxxx series
//  res = modem.sendSMS_UTF16(SMS_TARGET, u"Привіііт!", 9);
//  DBG("UTF16 SMS:", res ? "OK" : "fail");
#endif

#if defined(CALL_TARGET)
  DBG("Calling:", CALL_TARGET);

  // This is NOT supported on M590
  bool rescall = modem.callNumber(CALL_TARGET);
  DBG("Call:", rescall ? "OK" : "fail");

  if (rescall) {
    delay(5000L);

    // Play DTMF A, duration 1000ms
    modem.dtmfSend('A', 1000);

    // Play DTMF 0..4, default duration (100ms)
    for (char tone='0'; tone<='4'; tone++) {
      modem.dtmfSend(tone);
    }

    delay(5000);

    rescall = modem.callHangup();
    DBG("Hang up:", rescall ? "OK" : "fail");
  }
#endif

//  modem.gprsDisconnect();
//  if (!modem.isGprsConnected()) {
//    DBG("GPRS disconnected");
//  } else {
//    DBG("GPRS disconnect: Failed.");
//  }

  // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
  modem.poweroff();
  DBG("Poweroff.");

  // Do nothing forevermore
  while (true) {
    modem.maintain();
  }
}
