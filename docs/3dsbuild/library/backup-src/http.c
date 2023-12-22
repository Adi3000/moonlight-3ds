/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015 Iwan Timmer
 *
 * Moonlight is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Moonlight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
 */

#include "system/util/http.h"
#include "system/util/errors.h"

#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>

static CURL *curl;

static const char *pCertFile = "./client.pem";
static const char *pKeyFile = "./key.pem";
const char* gs_error;
static bool debug;

static size_t _write_curl(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  PHTTP_DATA mem = (PHTTP_DATA)userp;

  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL)
    return 0;

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

int http_init(int logLevel) {
  curl = curl_easy_init();
  debug = logLevel >= 2;
  if (!curl)
    return GS_FAILED;

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _write_curl);
  curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

  return GS_OK;
}

int http_request(char* url, PHTTP_DATA data) {
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
  curl_easy_setopt(curl, CURLOPT_URL, url);

  if (debug)
    printf("Request %s\n", url);

  if (data->size > 0) {
    free(data->memory);
    data->memory = (char*)malloc(1);
    if(data->memory == NULL)
      return GS_OUT_OF_MEMORY;

    data->size = 0;
  }
  CURLcode res = curl_easy_perform(curl);

  if(res != CURLE_OK) {
    gs_error = curl_easy_strerror(res);
    printf("Error : \n[%d] %s", res, gs_error);
    return GS_FAILED;
  } else if (data->memory == NULL) {
    return GS_OUT_OF_MEMORY;
  }

  if (debug)
    printf("Response:\n%s\n\n", data->memory);

  return GS_OK;
}

void http_cleanup() {
  curl_easy_cleanup(curl);
}

PHTTP_DATA http_create_data() {
  PHTTP_DATA data = (HTTP_DATA*)malloc(sizeof(HTTP_DATA));
  if (data == NULL)
    return NULL;

  data->memory = (char*)malloc(1);
  if(data->memory == NULL) {
    free(data);
    return NULL;
  }
  data->size = 0;

  return data;
}

void http_free_data(PHTTP_DATA data) {
  if (data != NULL) {
    if (data->memory != NULL)
      free(data->memory);

    free(data);
  }
}
