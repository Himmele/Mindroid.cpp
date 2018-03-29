/*
 * Copyright (C) 2017 E.S.R. Labs
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

#include <mindroid/net/Inet4Address.h>
#include <mindroid/lang/ByteArray.h>

namespace mindroid {

sp<InetAddress> Inet4Address::ANY = new Inet4Address(ByteArray::valueOf({ 0, 0, 0, 0 }), nullptr);

sp<InetAddress> Inet4Address::ALL = new Inet4Address(ByteArray::valueOf({ 255, 255, 255, 255 }), nullptr);

sp<InetAddress> Inet4Address::LOOPBACK = new Inet4Address(ByteArray::valueOf({ 127, 0, 0, 1 }), "localhost");

} /* namespace mindroid */
