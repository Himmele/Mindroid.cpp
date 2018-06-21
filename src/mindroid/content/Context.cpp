/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (C) 2013 Daniel Himmelein
 * Copyright (C) 2016 E.S.R.Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <mindroid/content/Context.h>
#include <mindroid/net/URI.h>

namespace mindroid {

const sp<URI> Context::SERVICE_MANAGER = URI::create("mindroid://serviceManager");
const sp<URI> Context::PACKAGE_MANAGER = URI::create("mindroid://packageManager");
const sp<URI> Context::LOGGER_SERVICE = URI::create("mindroid://logger");
const sp<URI> Context::CONSOLE_SERVICE = URI::create("mindroid://console");
const sp<URI> Context::ALARM_MANAGER = URI::create("mindroid://alarmManager");
const sp<URI> Context::POWER_MANAGER = URI::create("mindroid://powerManager");
const sp<URI> Context::LOCATION_SERVICE = URI::create("mindroid://locationService");
const sp<URI> Context::AUDIO_SERVICE = URI::create("mindroid://audioService");
const sp<URI> Context::TELEPHONY_SERVICE = URI::create("mindroid://telephonyService");
const sp<URI> Context::MEDIA_PLAYER_SERVICE = URI::create("mindroid://mediaPlayerService");
const sp<URI> Context::MESSAGE_BROKER = URI::create("mindroid://messageBroker");
const sp<URI> Context::VOICE_SERVICE = URI::create("mindroid://voiceService");

} /* namespace mindroid */
