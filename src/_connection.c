/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015-2017 Iwan Timmer
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

#include "connection.h"

#include <stdio.h>
#include <stdarg.h>
#include <signal.h>

#ifdef __3DS__
#include "3ds/3ds.h"
#endif

#ifdef HAVE_SDL
#include <SDL.h>
#endif

pthread_t main_thread_id = 0;
bool connection_debug;
ConnListenerRumble rumble_handler = NULL;

static void connection_terminated(int errorCode) {
  switch (errorCode) {
  case ML_ERROR_GRACEFUL_TERMINATION:
    printf("Connection has been terminated gracefully.\n");
#ifdef __3DS__
    sprintf(message_buffer, "Connection has been terminated gracefully.\n");
    is_error = 0;
#endif
    break;
  case ML_ERROR_NO_VIDEO_TRAFFIC:
    printf("No video received from host. Check the host PC's firewall and port forwarding rules.\n");
#ifdef __3DS__
    sprintf(message_buffer, "No video received from host.\n Check the host PC's firewall and port forwarding rules.\n");
    is_error = 1;
#endif
    break;
  case ML_ERROR_NO_VIDEO_FRAME:
    printf("Your network connection isn't performing well. Reduce your video bitrate setting or try a faster connection.\n");
#ifdef __3DS__
    sprintf(message_buffer, "Your network connection isn't performing well.\n Reduce your video bitrate setting or try a faster connection.\n");
    is_error = 1;
#endif
    break;
  case ML_ERROR_UNEXPECTED_EARLY_TERMINATION:
    printf("The connection was unexpectedly terminated by the host due to a video capture error. Make sure no DRM-protected content is playing on the host.\n");
#ifdef __3DS__
    sprintf(message_buffer, "The connection was unexpectedly terminated by the host due to a video capture error.\n Make sure no DRM-protected content is playing on the host.\n");
    is_error = 1;
#endif
    break;
  case ML_ERROR_PROTECTED_CONTENT:
    printf("The connection was terminated by the host due to DRM-protected content. Close any DRM-protected content on the host and try again.\n");
#ifdef __3DS__
    sprintf(message_buffer, "The connection was terminated by the host due to DRM-protected content.\n Close any DRM-protected content on the host and try again.\n");
    is_error = 1;
#endif
    break;
  default:
    printf("Connection terminated with error: %d\n", errorCode);
#ifdef __3DS__
    sprintf(message_buffer, "Connection terminated with error: %d\n", errorCode);
    is_error = 1;
#endif
    break;
  }

#ifndef __3DS__
  #ifdef HAVE_SDL
      SDL_Event event;
      event.type = SDL_QUIT;
      SDL_PushEvent(&event);
  #endif

  if (main_thread_id != 0)
    pthread_kill(main_thread_id, SIGTERM);
#else
  state = STATE_STOP_STREAM;
#endif
}

static void connection_log_message(const char* format, ...) {
  va_list arglist;
  va_start(arglist, format);
  vprintf(format, arglist);
  va_end(arglist);
}

static void rumble(unsigned short controllerNumber, unsigned short lowFreqMotor, unsigned short highFreqMotor) {
  if (rumble_handler)
    rumble_handler(controllerNumber, lowFreqMotor, highFreqMotor);
}

static void connection_status_update(int status) {
  switch (status) {
    case CONN_STATUS_OKAY:
      printf("Connection is okay\n");
      break;
    case CONN_STATUS_POOR:
      printf("Connection is poor\n");
      break;
  }
}

CONNECTION_LISTENER_CALLBACKS connection_callbacks = {
  .stageStarting = NULL,
  .stageComplete = NULL,
  .stageFailed = NULL,
  .connectionStarted = NULL,
  .connectionTerminated = connection_terminated,
  .logMessage = connection_log_message,
  .rumble = rumble,
  .connectionStatusUpdate = connection_status_update
};
