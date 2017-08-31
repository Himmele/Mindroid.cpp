/*
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

#include "mindroid/util/logging/FileHandler.h"
#include "mindroid/os/Environment.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <string>
#include <fstream>

namespace mindroid {

const sp<String> FileHandler::DEFAULT_PATTERN = String::valueOf("%h/Mindroid-%g.log");
const sp<String> FileHandler::CRLF = String::valueOf("\r\n");
const sp<String> FileHandler::DATA_VOLUME = String::valueOf("dataVolume");

FileHandler::FileHandler() {
    init(nullptr, false, 0, 0, 0, 0);
}

void FileHandler::init(const sp<String>& pattern, bool append, int32_t limit, int32_t count, int32_t bufferSize, int32_t dataVolumeLimit) {
    initProperties(pattern, append, limit, count, bufferSize, dataVolumeLimit);
    initOutputFiles();
    initPreferences();
}

void FileHandler::initProperties(const sp<String>& p, bool a, int32_t l, int32_t c, int32_t bufferSize, int32_t dataVolumeLimit) {
    mPattern = (p == nullptr) ? DEFAULT_PATTERN : p;
    if (mPattern == nullptr || (mPattern->length() == 0)) {
        Assert::fail("Pattern cannot be null or empty");
    }
    mAppend = a;
    mCount = c;
    mLimit = l;
    mBufferSize = bufferSize;
    mDataVolumeLimit = dataVolumeLimit;
    mCount = mCount < 1 ? DEFAULT_COUNT : mCount;
    mLimit = mLimit < 0 ? DEFAULT_LIMIT : mLimit;
    mBufferSize = mBufferSize < 0 ? 0 : mBufferSize;
    mDataVolumeLimit = mDataVolumeLimit < 0 ? 0 : mDataVolumeLimit;
}

void FileHandler::initOutputFiles() {
    for (int32_t generation = 0; generation < mCount; generation++) {
        mFiles->add(new File(parseFileName(generation)));
    }
    mFileName = mFiles->get(0)->getPath();
    if (mFiles->get(0)->exists() && (!mAppend || mFiles->get(0)->length() >= size_t(mLimit))) {
        for (int32_t i = mCount - 1; i > 0; i--) {
            if (mFiles->get(i)->exists()) {
                mFiles->get(i)->remove();
            }
            mFiles->get(i - 1)->renameTo(mFiles->get(i));
        }
    }
    mWriter = new Writer(mFiles->get(0), mAppend);
}

void FileHandler::initPreferences() {
    if (mDataVolumeLimit > 0) {
        sp<String> fileName = String::format("%x%s", (int32_t) mFiles->get(0)->getAbsolutePath()->hashCode(), ".xml");
        mPreferences = Environment::getSharedPreferences(mFiles->get(0)->getParentFile(), fileName, 0);
        mDataVolume = mPreferences->getInt(DATA_VOLUME, 0);
    }
}

void FileHandler::findNextGeneration() {
    close();

    for (int32_t i = mCount - 1; i > 0; i--) {
        if (mFiles->get(i)->exists()) {
            mFiles->get(i)->remove();
        }
        mFiles->get(i - 1)->renameTo(mFiles->get(i));
    }
    mWriter = new Writer(mFiles->get(0), mAppend);
}

sp<String> FileHandler::parseFileName(uint32_t gen) {
    uint32_t curChar = 0;
    ssize_t nextChar = 0;
    bool hasGeneration = false;

    struct passwd *pw = getpwuid(getuid());
    sp<String> homePath = String::valueOf(pw->pw_dir);
    bool homePathHasSeparatorEnd = (homePath == nullptr ? false : homePath->endsWith(File::separator));

    sp<String> s = new String();
    mPattern = mPattern->replace('/', File::separatorChar);

    sp<String> value = mPattern;
    while ((nextChar = mPattern->indexOf('%', curChar)) >= 0) {
        if (size_t(++nextChar) < mPattern->length()) {
            switch (value->charAt(nextChar)) {
            case 'g':
                s = s->append(value, curChar, nextChar - curChar - 1)->append(String::valueOf(gen));
                hasGeneration = true;
                break;
            case 'h':
                s = s->append(value, curChar, nextChar - curChar - 1)->append(homePath);
                if (!homePathHasSeparatorEnd) {
                    s = s->append(File::separator);
                }
                if (size_t(nextChar + 1) < mPattern->length()) {
                    if (value->charAt(nextChar + 1) == File::separatorChar) {
                        ++nextChar;
                    }
                }
                break;
            case '%':
                s = s->append(value, curChar, nextChar - curChar - 1)->append("%");
                break;
            default:
                s = s->append(value, curChar, nextChar - curChar);
            }
            curChar = ++nextChar;
        }
    }

    s = s->append(value, curChar, value->length() - curChar);

    if (!hasGeneration && mCount > 1) {
        s = s->append(".")->append(String::valueOf(gen));
    }

    return s;
}

FileHandler::FileHandler(const sp<String>& pattern) :
        mLock(new ReentrantLock()) {
    if (pattern == nullptr || pattern->length() == 0) {
        Assert::fail("Pattern cannot be null or empty");
    }
    init(pattern, false, DEFAULT_LIMIT, DEFAULT_COUNT, 0, 0);
}

FileHandler::FileHandler(const sp<String>& pattern, bool append) :
        mLock(new ReentrantLock()) {
    if (pattern == nullptr || pattern->length() == 0) {
        Assert::fail("Pattern cannot be null or empty");
    }
    init(pattern, append, DEFAULT_LIMIT, DEFAULT_COUNT, 0, 0);
}

FileHandler::FileHandler(const sp<String>& pattern, int32_t limit, int32_t count) :
        mLock(new ReentrantLock()) {
    if (pattern == nullptr || pattern->length() == 0) {
        Assert::fail("Pattern cannot be null or empty");
    }
    if (limit < 0 || count < 1) {
        Assert::fail("limit < 0 || count < 1");
    }
    init(pattern, false, limit, count, 0, 0);
}

FileHandler::FileHandler(const sp<String>& pattern, int32_t limit, int32_t count, bool append) :
        mLock(new ReentrantLock()) {
    if (pattern == nullptr || pattern->length() == 0) {
        Assert::fail("Pattern cannot be null or empty");
    }
    if (limit < 0 || count < 1) {
        Assert::fail("limit < 0 || count < 1");
    }
    init(pattern, append, limit, count, 0, 0);
}

FileHandler::FileHandler(const sp<String>& pattern, int32_t limit, int32_t count, bool append, int32_t bufferSize, int32_t dataVolumeLimit) :
        mLock(new ReentrantLock()) {
    if (pattern == nullptr || pattern->length() == 0) {
        Assert::fail("Pattern cannot be null or empty");
    }
    if (limit < 0 || count < 1) {
        Assert::fail("limit < 0 || count < 1");
    }
    if (bufferSize < 0 || dataVolumeLimit < 0) {
        Assert::fail("bufferSize < 0 || dataVolumeLimit < 0");
    }
    init(pattern, append, limit, count, bufferSize, dataVolumeLimit);
}

void FileHandler::close() {
    flush();

    if (mWriter != nullptr) {
        mWriter->close();
        mWriter = nullptr;
    }
}

void FileHandler::flush() {
    if (mWriter != nullptr) {
        if (mDataVolumeLimit > 0) {
            mPreferences->edit()->putInt(DATA_VOLUME, mDataVolume)->commit();
        }

        mWriter->flush();
    }
}

void FileHandler::clear() {
    close();

    for (int i = 0; i < mCount; i++) {
        if (mFiles->get(0)->exists()) {
            mFiles->get(0)->remove();
        }
    }
    mWriter = new Writer(mFiles->get(0), mAppend);
}

void FileHandler::publish(const sp<LogBuffer::LogRecord>& record) {
    AutoLock autoLock(mLock);

    sp<String> logMessage = record->toString();
    int32_t logMessageSize = logMessage->length() + CRLF->length();

    if (mDataVolumeLimit > 0) {
        if (mDataVolume + logMessageSize > mDataVolumeLimit) {
            return;
        }
    }

    if (mLimit > 0 && (mWriter->size() + logMessageSize) >= mLimit) {
        findNextGeneration();
    }

    mWriter->write(logMessage);
    mWriter->newLine();
    mFlushSize += logMessageSize;
    mDataVolume += logMessageSize;
    if (mFlushSize >= mBufferSize) {
        flush();
        mFlushSize = 0;
    }
}

bool FileHandler::dump(const sp<String>& fileName) {
    AutoLock autoLock(mLock);

    if (fileName == nullptr) {
        return false;
    }

    sp<File> tempFile = new File(mFiles->get(0)->getParentFile(), String::format("%x%s", (int32_t) mFiles->get(0)->getAbsolutePath()->hashCode(), ".tmp"));
    if (!tempFile->createNewFile()) {
        return false;
    }

    flush();

    std::ofstream writer(tempFile->getPath()->c_str(), std::ofstream::out);
    for (int i = mCount - 1; i >= 0; i--) {
        if (!mFiles->get(i)->exists()) {
            continue;
        }

        sp<File> inputFile = mFiles->get(i);
        std::ifstream reader(inputFile->getPath()->c_str());
        std::string logMessage;
        while (std::getline(reader, logMessage)) {
            writer << logMessage;
        }
        reader.close();
    }
    writer.close();

    return tempFile->renameTo(new File(Environment::getLogDirectory(), fileName));
}

FileHandler::Writer::Writer(const sp<File>& file, bool append) {
    if (!file->exists()) {
        if (file->getParentFile() != nullptr && !file->getParentFile()->exists()) {
            file->getParentFile()->mkdirs();
        }
        file->createNewFile();
    }

    int flags = O_RDWR | O_APPEND;
    if (!append) {
        flags |= O_TRUNC;
    }
    mFd = ::open(file->getPath()->c_str(), flags, 0600);
    mSize = file->length();
}

FileHandler::Writer::~Writer() {
    close();
}

void FileHandler::Writer::write(const sp<String>& s) {
    write(s, 0, s->length());
}

void FileHandler::Writer::write(const sp<String>& s, uint32_t offset, size_t length) {
    const ssize_t size = ::write(mFd, s->c_str() + offset, length);
    if (size > 0) {
        mSize += size;
    }
}

void FileHandler::Writer::flush() {
    ::fsync(mFd);
}

void FileHandler::Writer::close() {
    if (mFd != -1) {
        ::close(mFd);
        mFd = -1;
    }
}

int32_t FileHandler::Writer::size() {
    return mSize;
}

void FileHandler::Writer::newLine() {
    write(CRLF);
}

} /* namespace mindroid */
