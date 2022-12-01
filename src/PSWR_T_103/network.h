#ifndef _NETWORK_H_
#define _NETWORK_H_

#define TELNET_PORT 23
#define MAX_CLIENTS 10
#define CATCLIENT 1

extern void wifiinit(void);
extern void network_loop(void);
extern void init_ota(void);
extern void init_telnet(void);

class Telnet
{
public:
  size_t write(int iclient, const uint8_t *buffer, size_t size);
  size_t writeAll(const uint8_t *buffer, size_t size);
  size_t readAll(const uint8_t *buffer, size_t size);
  int available(int iclient);
  uint8_t connected(int iclient);
  uint8_t Anyconnection();
  size_t read(int iclient, const uint8_t *buffer, size_t size);
  void print(int iclient =0 , String string = "");

  void handle();
  void begin();
  Telnet(WiFiServer *Server, int maxconnections=1, String intro = "Connected to ESP-VPO PA0PHH\r\n");
  ~Telnet();

private:
  WiFiServer *pServer;
  WiFiClient *clients[MAX_CLIENTS] = { NULL };
  int max_connections;
  void CheckForConnections();
  void CheckForDroppedConnections();
  String IntroMessage;
};


extern Telnet DebugServer;
#ifndef CATCLIENT
extern Telnet CatServer;
#endif

#ifdef CATCLIENT
class TelnetClient
{
public:
  
  void handle();
  void begin(WiFiClient *client);
  size_t read(const uint8_t *buffer, size_t size);
  size_t write(const uint8_t *buffer, size_t size);
  uint8_t connected();
  uint8_t available();
  void  CheckForDroppedConnections();
private:
  WiFiClient *pclient =  NULL;
  

};

extern TelnetClient CatTelClient;
#endif

#endif
