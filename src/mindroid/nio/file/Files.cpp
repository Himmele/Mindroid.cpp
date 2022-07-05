/*
 * Copyright (C) 2022 E.S.R.Labs
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

#include <mindroid/nio/file/Files.h>
#include <sys/stat.h>

namespace mindroid {

bool Files::isSymbolicLink(const sp<File>& path) {
    struct stat status;
    if (lstat(path->getAbsolutePath ()->c_str(), &status) == 0) {
        return S_ISLNK(status.st_mode);
    } else {
        return false;
    }
}

} /* namespace mindroid */

