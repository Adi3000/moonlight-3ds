#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <3ds.h>
#include "system/client.hpp"
extern "C"
{
#include "system/util/errors.h"
#include "system/util/http.h"
#include <Limelight.h>
#include <3ds/services/httpc.h>
}
#include <pugixml.hpp>


PHTTP_DATA _httpc_get_call(char *url)
{
    Result res = httpcInit(0x10000); // Initialize HTTPC with 64KB of shared memory
    httpcContext context;
    u8 buffer[0x1000];
    u32 size = sizeof(buffer);
    u32 downloaded_size = (u32)malloc(1);
    std::stringstream processed_response_body;
    PHTTP_DATA processed_data;
    processed_data = http_create_data();
    if (R_FAILED(res)) {
        printf("Failed to initialize HTTPC: 0x%08lX\n", res);
        return NULL;
    }
    res = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 1);
    if (R_FAILED(res)) {
        printf("Failed to open HTTP context: 0x%08lX\n", res);
        return NULL;
    }
    res = httpcBeginRequest(&context);
    if (R_FAILED(res)) {
        printf("Failed to begin HTTP request: 0x%08lX\n", res);
        return NULL;
    }
    processed_data->size = 0;

    res = httpcDownloadData(&context, buffer, size, &downloaded_size);
    if (R_FAILED(res)) {
        printf("Failed to download data: 0x%08lX\n", res);
        return NULL;
    }
    processed_data->size += downloaded_size;
    if (downloaded_size > 0){
        processed_response_body << buffer;
        processed_data->memory = new char[processed_data->size+1];
        std::strcpy(processed_data->memory, processed_response_body.str().c_str());
        res = httpcCloseContext(&context);
        if (R_FAILED(res)) {
            printf("Failed to close HTTP context: 0x%08lX\n", res);
            return NULL;
        }
    }

    return processed_data;
}


PHTTP_DATA _curl_get_call(char *url)
{
    int error_code = http_init(3);
    printf("Prepare request...");
    PHTTP_DATA curl_response = http_create_data();
    printf("Created http data...");
    if (curl_response == NULL || error_code != GS_OK )
    {
        error_code = GS_OUT_OF_MEMORY;
        return NULL;
    }
    printf("Init Request OK...");
    if (http_request(url, curl_response) != GS_OK)
    {
        printf("Error on request ...");
        error_code = GS_IO_ERROR;
        return NULL;
    }
    printf("Displaying response");
    printf("Response of : %d bytes", curl_response->size);
    return curl_response;
}

void Client::parse_server_conf(PHTTP_DATA response_data) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(response_data->memory, sizeof(response_data->memory));
    printf("\nReady to connect [%s]!", result.description());
    this->server_conf->serverInfo.serverInfoAppVersion = doc.first_element_by_path("root/appversion").text().get();
    this->server_conf->serverInfo.serverInfoGfeVersion = doc.first_element_by_path("root/GfeVersion").text().get();
    this->server_conf->currentGame = doc.first_element_by_path("root/currentgame").text().as_int();
    this->server_conf->paired = strcmp(doc.first_element_by_path("root/PairStatus").text().get(), "1") == 0;
    this->server_conf->httpsPort = doc.first_element_by_path("root/HttpsPort").text().as_int();
    this->server_conf->gpuType = const_cast<char*>(doc.first_element_by_path("root/gputype").text().get());
    this->server_conf->gsVersion = const_cast<char*>(doc.first_element_by_path("root/GsVersion").text().get());
}

int Client::connect()
{

    PHTTP_DATA response_data = NULL;
    if (this->is_connected == 1)
    {
        printf("Already connected ");
    } else {
        printf("Initialize server...");
        char *url = "http://192.168.0.21:47989/serverinfo?uniqueid=7f98f93d-7513-4857-8f37-3016c2eaa7a8";
        response_data = _httpc_get_call(url);
        if(response_data != NULL) {
            memset(this->server_conf, 0, sizeof(*this->server_conf));
            LiInitializeServerInformation(&this->server_conf->serverInfo);
            this->server_conf->serverInfo.address = "192.168.0.21";
            //this->parse_server_conf(response_data);
            /*printf("\nGS Version : %s", this->server_conf->gsVersion);
            printf("\nGPU Type : %s",this->server_conf->gpuType);
            printf("\nGfeVersion : %s",  this->server_conf->serverInfo.serverInfoGfeVersion);
            printf("\nAppVersion : %s", this->server_conf->serverInfo.serverInfoAppVersion); */
            printf("Cleaning OK");
            http_free_data(response_data);
        }
    }

    return this->is_connected;
}

