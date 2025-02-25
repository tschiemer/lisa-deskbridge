/**
* L-ISA Deskbridge
* Copyright (C) 2025  Philip Tschiemer, https://github.com/tschiemer/lisa-deskbridge
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LISA_DESKBRIDGE_LOG_H
#define LISA_DESKBRIDGE_LOG_H

#include <cstdio>
#include <functional>

namespace LisaDeskbridge {

    enum LogLevel {
        LogLevelNone    = 0,
        LogLevelError   = 1,
        LogLevelInfo    = 2,
        LogLevelDebug   = 3
    };

    typedef void (*LogFunction)(LogLevel,const char *,...);

    LogLevel getLogLevel();
    void setLogLevel(LogLevel level);
    void setLogFile(FILE * file);
    void setLogFunction(LogFunction function);

    void defaultLog(LogLevel level, const char * msg, ...);

    void logError(const char * msg, ...);

    extern LogFunction log;
}


#endif //LISA_DESKBRIDGE_LOG_H
