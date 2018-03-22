/*
 * Copyright (C) 2017 E.S.R.Labs
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

#include <mindroid/io/OutputStream.h>
#include <mindroid/util/Assert.h>

namespace mindroid {

void OutputStream::write(const sp<ByteArray>& buffer, size_t offset, size_t count) {
    Assert::assertNotNull(buffer);
    Assert::assertFalse<IndexOutOfBoundsException>((offset < 0) || (count < 0) || ((offset + count) > buffer->size()));

    if (count == 0) {
        return;
    }
    for (size_t i = offset; i < offset + count; i++) {
        write(buffer->get(i));
    }
}

} /* namespace mindroid */
