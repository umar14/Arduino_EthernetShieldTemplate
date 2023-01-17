// Demonstrate how to use FlashStorage_SAMD with an API that is similar to the EEPROM library to Store and retrieve structured data.
// #define EEPROM_EMULATION_SIZE     (4 * 1024)

// Use 0-2. Larger for more debugging messages
#define FLASH_DEBUG 0
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include <FlashStorage_SAMD.h>
#include <Arduino.h>
#include <Ethernet.h>

// #define ETH_RST     20 //not present on MKR ETH
#define ETH_CS 5

const char head[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8" />
    <title>Stream Control</title>
    <style>
      .grid-container {
        display: grid;
        grid-template-columns: auto auto auto auto;
        background-color: #2196f3;
        padding: 10px;
      }
      .grid-item {
        background-color: rgba(255, 255, 255, 0.8);
        border: 1px solid rgba(0, 0, 0, 0.8);
        padding: 20px;
        font-size: 30px;
        text-align: center;
        font-family: sans-serif;
      }
      .network {
        text-align: center;
      }
      .button {
        background-color: rgb(210, 101, 101);
        border: rgb(197, 119, 119);
        color: 2px solid #4CAF50;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 4px 2px;
        cursor: pointer;
        font-family: sans-serif;
        border-radius: 12px;
      }
      .button:hover {
        background-color: #4caf50;
        color: white;
      }
      h1 {
        text-align: center;
        font-size: 36px;
        color: rgb(221, 9, 9);
        font-family: Verdana;
      }
      label {
        color: rgb(2, 2, 2);
        font-family: Verdana;
        font-size: 90%;
      }
    </style>
  </head>
  <body>
    <h1>Stream Control</h1>
    <div class="grid-container">
      <div class="grid-item">Output A:</div>
      <div class="grid-item">Output B:</div>
      <div class="grid-item">Output C:</div>
      <div class="grid-item">Output D:</div>
        )=====";

const char formA[] PROGMEM = R"=====(
      <div class="grid-item">
        <form action="/">
          <label>Universe:</label> <input size="10" type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="a1" value="OutputA" /><br />
          <label>Colour:</label>  <select name="a2">
            <option value="0" selected="selected">Red</option>
            <option value="1">Orange</option>
            <option value="2">Yellow</option>
            <option value="3">Green</option>
            <option value="4">Fern</option>
            <option value="5">Cyan</option>
            <option value="6">Blue</option>
            <option value="7">Purple</option>
            <option value="8">White</option>
          </select><br>
          <label>Linking Key:</label> <input size=10 type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="a3" value="inkKeyA" /><br /><br>
          <input class="button" type="submit" value="CHANGE" />
        </form>
      </div>
      )=====";

const char formB[] PROGMEM = R"=====(   
      <div class="grid-item">
        <form action="/">
          <label>Universe:</label> <input size="10" type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="b1" value="OutputB" /><br />
          <label>Colour:</label>  <select name="b2">
            <option value="0" selected="selected">Red</option>
            <option value="1">Orange</option>
            <option value="2">Yellow</option>
            <option value="3">Green</option>
            <option value="4">Fern</option>
            <option value="5">Cyan</option>
            <option value="6">Blue</option>
            <option value="7">Purple</option>
            <option value="8">White</option>
          </select><br>
          <label>Linking Key:</label> <input size=10 type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="b3" value="inkKeyB" /><br /><br>
          <input class="button" type="submit" value="CHANGE" />
        </form>
      </div>
       )=====";

const char formC[] PROGMEM = R"=====(
      <div class="grid-item">
        <form action="/">
          <label>Universe:</label> <input size="10" type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="c1" value="OutputC" /><br />
          <label>Colour:</label>  <select name="c2">
            <option value="0" selected="selected">Red</option>
            <option value="1">Orange</option>
            <option value="2">Yellow</option>
            <option value="3">Green</option>
            <option value="4">Fern</option>
            <option value="5">Cyan</option>
            <option value="6">Blue</option>
            <option value="7">Purple</option>
            <option value="8">White</option>
          </select><br>
          <label>Linking Key:</label> <input size=10 type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="c3" value="inkKeyC" /><br /><br>
          <input class="button" type="submit" value="CHANGE" />
        </form>
      </div>
      )=====";

const char formD[] PROGMEM = R"=====(
      <div class="grid-item">
        <form action="/">
          <label>Universe:</label> <input size="10" type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="d1" value="OutputD" /><br />
          <label>Colour:</label>  <select name="d2">
            <option value="0" selected="selected">Red</option>
            <option value="1">Orange</option>
            <option value="2">Yellow</option>
            <option value="3">Green</option>
            <option value="4">Fern</option>
            <option value="5">Cyan</option>
            <option value="6">Blue</option>
            <option value="7">Purple</option>
            <option value="8">White</option>
          </select><br>
          <label>Linking Key:</label> <input size=10 type="text" pattern="^[1-6]{1}[0-9]{0,3}$" name="d3" value="inkKeyD" /><br /><br>
          <input class="button" type="submit" value="CHANGE" />
        </form>
      </div>
      )=====";

const char foot[] PROGMEM = R"=====(
      <div class="grid-item">StatusA</div>
      <div class="grid-item">StatusB</div>
      <div class="grid-item">StatusC</div>
      <div class="grid-item">StatusD</div>
      <div class="grid-item">
        <form action="/">
          <input class="button" name="SYNCA" type="submit" value="SYNC" />
        </form>
      </div>
      <div class="grid-item">
        <form action="/">
          <input class="button" name="SYNCB" type="submit" value="SYNC" />
        </form>
      </div>
      <div class="grid-item">
        <form action="/">
          <input class="button" name="SYNCC" type="submit" value="SYNC" />
        </form>
      </div>
      <div class="grid-item">
        <form action="/">
          <input class="button" name="SYNCD" type="submit" value="SYNC" />
        </form>
      </div>
    </div>
    <div class="network">
      <h1>Network Settings</h1>
      <form action="/">
        <label for="ip">IP Address:</label>
        <input type="text" pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$" name="ip" value="VAR1" /><br /><br />
        <label for="subnet">Subnet:</label>
        <input type="text" pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$" name="subnet" value="VAR2" /><br /><br />
        <label for="gw">Gateway:</label>
        <input type="text" pattern="^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$" name="gw" value="VAR3" /><br /><br />
        <label for="mac">MAC Address:</label>
        <input type="text" pattern="^([0-9A-Fa-f]{2}[-]){5}([0-9A-Fa-f]{2})$" name="mac" value="VAR4" /><br /><br />
        <input class="button" type="submit" value="CHANGE" />
      </form>
    </div>
  </body>
</html>
)=====";

// uint8_t mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48}; // MAC Adress of your device
// uint8_t mac[6]; // for use with generator

String Update_Response = "HTTP/1.1 200 OK\n Content-Type: text/html\n\n  <meta http-equiv=\"Refresh\" content=\"0; url='https://IP'\" />";

// SYNC BUTTONS
bool SYNC_A = false;
bool SYNC_B = false;
bool SYNC_C = false;
bool SYNC_D = false;

// Default Statuses
String Status_A = "OFF";
String Status_B = "OFF";
String Status_C = "OFF";
String Status_D = "OFF";

# define COLOR 1
// Example -> Call color_array(a[COLOR]) to get color for A
// Example -> Call color_array(b[COLOR]) to get color for B
// Example -> Call color_array(c[COLOR]) to get color for C
// Example -> Call color_array(d[COLOR]) to get color for D

String color_array[9] = {
  "RED",
  "ORANGE",
  "YELLOW",
  "GREEN",
  "FERN",
  "CYAN",
  "BLUE",
  "PURPLE",
  "WHITE"};

enum Colors
{
  RED,
  ORANGE,
  YELLOW,
  GREEN,
  FERN,
  CYAN,
  BLUE,
  PURPLE,
  WHITE
};

// Default Colors
enum Colors colorA = RED;
enum Colors colorB = BLUE;
enum Colors colorC = WHITE;
enum Colors colorD = GREEN;

// Default Values
uint16_t a[3]   = {6999, colorA, 6999};
uint16_t b[3]   = {6999, colorB, 6999};
uint16_t cc[3]  = {6999, colorC, 6999};
uint16_t d[3]   = {6999, colorD, 6999};

typedef struct
{
  uint8_t ip[4];
  uint8_t subnet[4];
  uint8_t gateway[4];
  uint8_t mac[6];
} NetworkSettings;

NetworkSettings default_address = {
    {192, 168, 1, 201},
    {255, 255, 0, 0},
    {192, 168, 1, 1},
    {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}};

NetworkSettings input_ip_web;
NetworkSettings stored_address;

IPAddress current_ip(0, 0, 0, 0);
IPAddress current_gateway(0, 0, 0, 0);
IPAddress current_subnet(0, 0, 0, 0);
IPAddress dns(192, 168, 1, 1);
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
String hexString = "";

EthernetServer server(80);

int signature;
int signature1;
int signature2;
int signature3;
int signature4;
uint16_t storedAddress = 0;
uint16_t storedAddress1 = 30;
uint16_t storedAddress2 = 50;
uint16_t storedAddress3 = 70;
uint16_t storedAddress4 = 90;
const int WRITTEN_SIGNATURE = 0xBEEFDEED;
const int WRITTEN_SIGNATURE1 = 0xAEEADEED;
const int WRITTEN_SIGNATURE2 = 0xBEEBDEED;
const int WRITTEN_SIGNATURE3 = 0xCEECDEED;
const int WRITTEN_SIGNATURE4 = 0xDEEDDEED;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  EEPROM.get(storedAddress1, signature1);

  if (signature1 == WRITTEN_SIGNATURE1)
  {
    Serial.println("EEPROM has 'A' data, reading stored 'A' data");
    EEPROM.get(storedAddress1 + sizeof(signature1), a);
    Serial.print("a1:");
    Serial.println(a[0]);
    Serial.print("a2:");
    Serial.println(a[1]);
    Serial.print("a3:");
    Serial.println(a[2]);
  }
  else
  {
    Serial.println("EEPROM is empty, writing default 'A' data");
    EEPROM.put(storedAddress1, WRITTEN_SIGNATURE1);
    delay(200);
    EEPROM.put(storedAddress1 + sizeof(signature1), a);
    delay(200);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }

  EEPROM.get(storedAddress2, signature2);

  if (signature2 == WRITTEN_SIGNATURE2)
  {
    Serial.println("EEPROM has 'B' data, reading stored 'B' data");
    EEPROM.get(storedAddress2 + sizeof(signature2), b);
    Serial.print("b1:");
    Serial.println(b[0]);
    Serial.print("b2:");
    Serial.println(b[1]);
    Serial.print("b3:");
    Serial.println(b[2]);
  }
  else
  {
    Serial.println("EEPROM is empty, writing default 'B' data");
    EEPROM.put(storedAddress2, WRITTEN_SIGNATURE2);
    delay(200);
    EEPROM.put(storedAddress2 + sizeof(signature2), b);
    delay(200);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }

  EEPROM.get(storedAddress3, signature3);

  if (signature3 == WRITTEN_SIGNATURE3)
  {
    Serial.println("EEPROM has 'C' data, reading stored 'C' data");
    EEPROM.get(storedAddress3 + sizeof(signature3), cc);
    Serial.print("c1:");
    Serial.println(cc[0]);
    Serial.print("c2:");
    Serial.println(cc[1]);
    Serial.print("c3:");
    Serial.println(cc[2]);
  }
  else
  {
    Serial.println("EEPROM is empty, writing default 'C' data");
    EEPROM.put(storedAddress3, WRITTEN_SIGNATURE3);
    delay(200);
    EEPROM.put(storedAddress3 + sizeof(signature3), cc);
    delay(200);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }

  EEPROM.get(storedAddress4, signature4);

  if (signature4 == WRITTEN_SIGNATURE4)
  {
    Serial.println("EEPROM has 'D' data, reading stored 'D' data");
    EEPROM.get(storedAddress4 + sizeof(signature4), d);
    Serial.print("d1:");
    Serial.println(d[0]);
    Serial.print("d2:");
    Serial.println(d[1]);
    Serial.print("d3:");
    Serial.println(d[2]);
  }
  else
  {
    Serial.println("EEPROM is empty, writing default 'D' data");
    EEPROM.put(storedAddress4, WRITTEN_SIGNATURE4);
    delay(200);
    EEPROM.put(storedAddress4 + sizeof(signature4), d);
    delay(200);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }
  }

  EEPROM.get(storedAddress, signature);

  if (signature == WRITTEN_SIGNATURE)
  {
    Serial.println("EEPROM has IP data, reading stored IP data");

    EEPROM.get(storedAddress + sizeof(signature), stored_address);

    current_ip[0] = stored_address.ip[0];
    current_ip[1] = stored_address.ip[1];
    current_ip[2] = stored_address.ip[2];
    current_ip[3] = stored_address.ip[3];

    current_gateway[0] = stored_address.gateway[0];
    current_gateway[1] = stored_address.gateway[1];
    current_gateway[2] = stored_address.gateway[2];
    current_gateway[3] = stored_address.gateway[3];

    current_subnet[0] = stored_address.subnet[0];
    current_subnet[1] = stored_address.subnet[1];
    current_subnet[2] = stored_address.subnet[2];
    current_subnet[3] = stored_address.subnet[3];

    mac[0] = stored_address.mac[0];
    mac[1] = stored_address.mac[1];
    mac[2] = stored_address.mac[2];
    mac[3] = stored_address.mac[3];
    mac[4] = stored_address.mac[4];
    mac[5] = stored_address.mac[5];
  }
  else
  {
    Serial.println("EEPROM is empty, writing default IP data");

    EEPROM.put(storedAddress, WRITTEN_SIGNATURE);
    delay(200);
    EEPROM.put(storedAddress + sizeof(signature), default_address);
    delay(200);

    if (!EEPROM.getCommitASAP())
    {
      Serial.println("CommitASAP not set. Need commit()");
      EEPROM.commit();
    }

    current_ip[0] = default_address.ip[0];
    current_ip[1] = default_address.ip[1];
    current_ip[2] = default_address.ip[2];
    current_ip[3] = default_address.ip[3];

    current_gateway[0] = default_address.gateway[0];
    current_gateway[1] = default_address.gateway[1];
    current_gateway[2] = default_address.gateway[2];
    current_gateway[3] = default_address.gateway[3];

    current_subnet[0] = default_address.subnet[0];
    current_subnet[1] = default_address.subnet[1];
    current_subnet[2] = default_address.subnet[2];
    current_subnet[3] = default_address.subnet[3];

    mac[0] = default_address.mac[0];
    mac[1] = default_address.mac[1];
    mac[2] = default_address.mac[2];
    mac[3] = default_address.mac[3];
    mac[4] = default_address.mac[4];
    mac[5] = default_address.mac[5];
  }

  Serial.println("IP:" + DisplayAddress(current_ip));
  Serial.println("SUBNET:" + DisplayAddress(current_subnet));
  Serial.println("GATEWAY:" + DisplayAddress(current_gateway));

  for (int i = 0; i < 6; i++)
  {
    hexString += String(mac[i], HEX);
    if (i < 5)
      hexString += "-";
  }

  Serial.println("MAC: " + hexString);

  Ethernet.init(ETH_CS);
  Ethernet.begin(mac, current_ip, dns, current_gateway, current_subnet);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true)
    {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF)
  {
    Serial.println("Ethernet cable is not connected.");
  }

  // start listening for clients
  server.begin();
}

String DisplayAddress(IPAddress address)
{
  return String(address[0]) + "." +
         String(address[1]) + "." +
         String(address[2]) + "." +
         String(address[3]);
}

int StrToHex(char str[])
{
  return (int)strtol(str, 0, 16);
}

void tokenize(char *val1, char *val2, char *val3, char *str)
{
  int k = 0, m = 0, count = 0;
  int start = 0;
  char buff[6];

  char token = '&';

  for (int j = 0; j < strlen(str); j++)
  {
    if (str[j] == '=')
    {
      start = 1;
      continue;
    }
    if (start)
    {
      if (str[j] == token) // NO input value or value end
      {
        start = 0;
        m = 0;

        if (count == 0)
          strcpy(val1, buff);
        if (count == 1)
        {
          strcpy(val2, buff);
          token = 'H';
        }
        if (count == 2)
        {
          strcpy(val3, buff);
          break;
        }
        memset(buff, '\0', 6); // Empty buff
        count++;               // Value stored
      }
      else
      {
        buff[m] = str[j];
        m++;
      }
    }
  }
}

void loop()
{
  if (SYNC_A)
  {
    SYNC_A = false;
    Serial.println("SYNC-A Button Pressed!");
    if (Status_A == "OFF")
      Status_A = "ON";
    else
      Status_A = "OFF";
  }

  if (SYNC_B)
  {
    SYNC_B = false;
    Serial.println("SYNC-B Button Pressed!");
    if (Status_B == "OFF")
      Status_B = "ON";
    else
      Status_B = "OFF";
  }

  if (SYNC_C)
  {
    SYNC_C = false;
    Serial.println("SYNC-C Button Pressed!");
    if (Status_C == "OFF")
      Status_C = "ON";
    else
      Status_C = "OFF";
  }

  if (SYNC_D)
  {
    SYNC_D = false;
    Serial.println("SYNC-D Button Pressed!");
    if (Status_D == "OFF")
      Status_D = "ON";
    else
      Status_D = "OFF";
  }

  listenForEthernetClients();
}

void listenForEthernetClients()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    // Serial.println("Got a client");
    // an HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    String line = "";
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        // Serial.print(c);
        line = line + c;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print(head);

          String tmp1 = formA;
          String tmp2 = formB;
          String tmp3 = formC;
          String tmp4 = formD;
          
          String footer = foot;
          String clr = "";
          
          tmp1.replace("OutputA", String(a[0])); 
          clr = "value=\"" + String(a[1]) + "\"" + " selected=\"selected\"";
          tmp1.replace("selected=\"selected\"", ""); // Clear Last Selected
          tmp1.replace("value=\"" + String(a[1]) + "\"", clr);
          tmp1.replace("inkKeyA", String(a[2])); 
          
          tmp2.replace("OutputB", String(b[0]));
          clr = "value=\"" + String(b[1]) + "\"" + " selected=\"selected\"";
          tmp2.replace("selected=\"selected\"", ""); // Clear Last Selected
          tmp2.replace("value=\"" + String(b[1]) + "\"", clr);
          tmp2.replace("inkKeyB", String(b[2]));
          
          tmp3.replace("OutputC", String(cc[0]));
          clr = "value=\"" + String(cc[1]) + "\"" + " selected=\"selected\"";
          tmp3.replace("selected=\"selected\"", ""); // Clear Last Selected
          tmp3.replace("value=\"" + String(cc[1]) + "\"", clr);
          tmp3.replace("inkKeyC", String(cc[2]));
          
          tmp4.replace("OutputD", String(d[0]));
          clr = "value=\"" + String(d[1]) + "\"" + " selected=\"selected\"";
          tmp4.replace("selected=\"selected\"", ""); // Clear Last Selected
          tmp4.replace("value=\"" + String(d[1]) + "\"", clr);
          tmp4.replace("inkKeyD", String(d[2]));
          
          footer.replace("StatusA", Status_A);
          footer.replace("StatusB", Status_B);
          footer.replace("StatusC", Status_C);
          footer.replace("StatusD", Status_D);
          footer.replace("VAR1", DisplayAddress(Ethernet.localIP()));
          footer.replace("VAR2", DisplayAddress(Ethernet.subnetMask()));
          footer.replace("VAR3", DisplayAddress(Ethernet.gatewayIP()));
          footer.replace("VAR4", hexString);

          client.print(tmp1);
          client.print(tmp2);
          client.print(tmp3);
          client.print(tmp4);
          client.print(footer);

          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
          int len = line.length();

          if (line.startsWith("GET /?ip"))
          {
            Serial.print(line);

            char req[len + 1];
            line.toCharArray(req, len);

            char *pch;
            int i = 0;
            pch = strtok(req, "=");

            IPAddress ip;
            IPAddress sn;
            IPAddress gw;

            while (pch != NULL)
            {
              if (i == 1)
              {
                ip.fromString(pch);
              }
              else if (i == 3)
              {
                sn.fromString(pch);
              }
              else if (i == 5)
              {
                gw.fromString(pch);
              }
              else if (i == 7)
              {
                char *kch;
                kch = strtok(pch, "-");
                int x = 0;
                while (kch != NULL)
                {
                  mac[x] = StrToHex(kch);
                  kch = strtok(NULL, "-");
                  x++;
                }
              }
              else
              {
                delay(1);
              }

              pch = strtok(NULL, " =&");
              i++;
            }

            input_ip_web.ip[0] = ip[0];
            input_ip_web.ip[1] = ip[1];
            input_ip_web.ip[2] = ip[2];
            input_ip_web.ip[3] = ip[3];
            input_ip_web.gateway[0] = gw[0];
            input_ip_web.gateway[1] = gw[1];
            input_ip_web.gateway[2] = gw[2];
            input_ip_web.gateway[3] = gw[3];
            input_ip_web.subnet[0] = sn[0];
            input_ip_web.subnet[1] = sn[1];
            input_ip_web.subnet[2] = sn[2];
            input_ip_web.subnet[3] = sn[3];
            input_ip_web.mac[0] = mac[0];
            input_ip_web.mac[1] = mac[1];
            input_ip_web.mac[2] = mac[2];
            input_ip_web.mac[3] = mac[3];
            input_ip_web.mac[4] = mac[4];
            input_ip_web.mac[5] = mac[5];

            EEPROM.put(storedAddress + sizeof(signature), input_ip_web);
            if (!EEPROM.getCommitASAP())
            {
              Serial.println("CommitASAP not set. Need commit()");
              EEPROM.commit();
            }
            else
            {
              Serial.println("Settings Stored!");
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("Settings Updated!<br>");
            client.print("New IP is: <a href=http://" + DisplayAddress(ip) + ">" + DisplayAddress(ip) + "</a>");

            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();

            Serial.println("Reseting Board with New IP Settings");
            delay(5);
            NVIC_SystemReset();
          }

          else if (line.startsWith("GET /?a1"))
          {
            Serial.print(line);

            char str[len + 1];
            line.toCharArray(str, len);

            char val1[6];
            char val2[6];
            char val3[6];
            int num;

            tokenize(val1, val2, val3, str);

            num = atoi(val1);
            if (num != 0)
              a[0] = num;

            num = atoi(val2);
            //if (num != 0)
              a[1] = num;

            num = atoi(val3);
            if (num != 0)
              a[2] = num;

            EEPROM.put(storedAddress1 + sizeof(signature1), a);
            if (!EEPROM.getCommitASAP())
            {
              Serial.println("CommitASAP not set. Need commit()");
              EEPROM.commit();
            }
            else
            {
              Serial.println("Settings Stored!");
            }

            Update_Response.replace("IP", DisplayAddress(current_ip));
            client.println(Update_Response);

            // client.println("HTTP/1.1 200 OK");
            // client.println("Content-Type: text/html");
            // client.println();
            // client.println("Settings Updated!<br>");
            // client.print("<a href=/>Go Back</a>");

            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?b1"))
          {
            Serial.print(line);

            char str[len + 1];
            line.toCharArray(str, len);

            char val1[6];
            char val2[6];
            char val3[6];
            int num;

            tokenize(val1, val2, val3, str);

            num = atoi(val1);
            if (num != 0)
              b[0] = num;

            num = atoi(val2);
            //if (num != 0)
              b[1] = num;

            num = atoi(val3);
            if (num != 0)
              b[2] = num;

            EEPROM.put(storedAddress2 + sizeof(signature2), b);
            if (!EEPROM.getCommitASAP())
            {
              Serial.println("CommitASAP not set. Need commit()");
              EEPROM.commit();
            }
            else
            {
              Serial.println("Settings Stored!");
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("Settings Updated!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?c1"))
          {
            Serial.print(line);

            char str[len + 1];
            line.toCharArray(str, len);

            char val1[6];
            char val2[6];
            char val3[6];
            int num;

            tokenize(val1, val2, val3, str);

            num = atoi(val1);
            if (num != 0)
              cc[0] = num;

            num = atoi(val2);
            // if (num != 0)
              cc[1] = num;

            num = atoi(val3);
            if (num != 0)
              cc[2] = num;

            EEPROM.put(storedAddress3 + sizeof(signature3), cc);
            if (!EEPROM.getCommitASAP())
            {
              Serial.println("CommitASAP not set. Need commit()");
              EEPROM.commit();
            }
            else
            {
              Serial.println("Settings Stored!");
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("Settings Updated!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?d1"))
          {
            Serial.print(line);

            char str[len + 1];
            line.toCharArray(str, len);

            char val1[6];
            char val2[6];
            char val3[6];
            int num;

            tokenize(val1, val2, val3, str);

            num = atoi(val1);
            if (num != 0)
              d[0] = num;

            num = atoi(val2);
            // if (num != 0)
              d[1] = num;

            num = atoi(val3);
            if (num != 0)
              d[2] = num;

            EEPROM.put(storedAddress4 + sizeof(signature4), d);
            if (!EEPROM.getCommitASAP())
            {
              Serial.println("CommitASAP not set. Need commit()");
              EEPROM.commit();
            }
            else
            {
              Serial.println("Settings Stored!");
            }

            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("Settings Updated!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?SYNCA"))
          {
            SYNC_A = true;
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("SYNC A Done!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?SYNCB"))
          {
            SYNC_B = true;
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("SYNC B Done!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?SYNCC"))
          {
            SYNC_C = true;
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("SYNC C Done!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else if (line.startsWith("GET /?SYNCD"))
          {
            SYNC_D = true;
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("SYNC D Done!<br>");
            client.print("<a href=/>Go Back</a>");
            // give the web browser time to receive the data
            delay(5);
            // close the connection:
            client.stop();
          }

          else
          {
            delay(1);
          }

          line = "";
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}