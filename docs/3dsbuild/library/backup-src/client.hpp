#include <3ds.h>
extern "C"
{
    #include <Limelight.h>
    #include "system/util/http.h"
}

typedef struct _DISPLAY_MODE {
  unsigned int height;
  unsigned int width;
  unsigned int refresh;
  struct _DISPLAY_MODE *next;
} DISPLAY_MODE, *PDISPLAY_MODE;

typedef struct _SERVER_DATA {
  char* gpuType;
  bool paired;
  bool supports4K;
  bool unsupported;
  int currentGame;
  int serverMajorVersion;
  char* gsVersion;
  PDISPLAY_MODE modes;
  SERVER_INFORMATION serverInfo;
  unsigned short httpPort;
  unsigned short httpsPort;
} SERVER_DATA, *PSERVER_DATA;



class Client {
    public:
        int is_connected;
        int connect();
        PSERVER_DATA server_conf;
        PSTREAM_CONFIGURATION streaming_conf;
    private:
        void parse_server_conf(PHTTP_DATA response_data);
};