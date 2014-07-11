/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++03
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      TGSDatabaseInterface.cpp
**
**      ------------------------------------------------------------------------
**
**      The MIT License (MIT)
**
**      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
**      associated documentation files (the "Software"), to deal in the Software without restriction,
**      including without limitation the rights to use, copy, modify, merge, publish, distribute,
**      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
**      furnished to do so, subject to the following conditions:
**      The above copyright notice and this permission notice shall be included in all copies or
**      substantial portions of the Software.
**      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
**      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
**      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
**      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
**      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
**      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
**      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
**      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
**      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
**      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
**      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
**      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/

#include "TGSDatabaseInterface.h"
#include <iostream>

namespace tgs {

/*protected */TGSDatabaseInterface::TGSDatabaseInterface(void) : _database(NULL)
{
}

/*public virtual */TGSDatabaseInterface::~TGSDatabaseInterface(void)
{
    close();
}

/*public virtual */bool TGSDatabaseInterface::isValid(void) const
{
    return (_database != NULL);
}

/*public virtual */TGSError TGSDatabaseInterface::open(std::string const& file, int timeout)
{
    TGSError error(TGSERROR_OK);
    
    if (_database == NULL) {
        if (sqlite3_open_v2(file.c_str(), &_database, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK) {
            if (sqlite3_busy_timeout(_database, timeout) == SQLITE_OK) {
                _statement = NULL;
            }
            else {
                std::cerr << "database busy timeout error [" << sqlite3_errmsg(_database) << "]" << std::endl;
                error = TGSERROR_FAILED;
            }
        }
        else {
            std::cerr << "database open error [" << sqlite3_errmsg(_database) << "]" << std::endl;
            error = TGSERROR_FAILED;
        }
        if (error != TGSERROR_OK) {
            self::close();
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public virtual */void TGSDatabaseInterface::close(void)
{
    if (_database != NULL) {
        if (_statement != NULL) {
            if (sqlite3_finalize(_statement) != SQLITE_OK) {
                std::cerr << "database finalize error [" << sqlite3_errmsg(_database) << "]" << std::endl;
            }
        }
        _statement = NULL;
        vacuum();
        if (sqlite3_close_v2(_database) != SQLITE_OK) {
            std::cerr << "database close error [" << sqlite3_errmsg(_database) << "]" << std::endl;
        }
    }
    _database = NULL;
    return;
}

/*public */TGSError TGSDatabaseInterface::begin(void)
{
    return execute("BEGIN EXCLUSIVE;");
}

/*public */void TGSDatabaseInterface::end(void)
{
    execute("END;");
    return;
}

/*public */TGSError TGSDatabaseInterface::vacuum(void)
{
    return execute("VACUUM;");
}

/*protected */TGSError TGSDatabaseInterface::create(std::string const& table, char const* (*format)[2], unsigned int length)
{
    std::string string;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (format != NULL && length > 0) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            for (i = 0; i < length; ++i) {
                string += format[i][0];
                string += " ";
                string += format[i][1];
                string += ", ";
            }
            if (string.size() >= 2) {
                string.erase(string.size() - 2);
            }
            error = execute("CREATE TABLE IF NOT EXISTS " + table + " (" + string + ");");
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::update(std::string const& table, char const* (*format)[2], unsigned int length, int primary, va_ref<std::string const> condition, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, condition);
    error = update(table, format, length, primary, static_cast<std::string const&>(condition), ap);
    va_end(ap);
    return error;
}

/*protected */TGSError TGSDatabaseInterface::update(std::string const& table, char const* (*format)[2], unsigned int length, int primary, std::string const& condition, va_list ap)
{
    va_list pp;
    std::string key;
    std::string name;
    std::string value;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (format != NULL && length > 0) {
        if (primary < 0 || primary < length) {
            if ((error = checkFlow()) == TGSERROR_OK) {
                if (primary >= 0) {
                    va_copy(pp, ap);
                    for (i = 0; i < primary; ++i) {
                        va_arg(pp, void*);
                    }
                    if ((error = bind(&format[primary], &key, pp)) == TGSERROR_OK) {
                        if ((error = countRow(table, NULL, "WHERE " + key, &i)) == TGSERROR_OK) {
                            if (i <= 0) {
                                primary = -1;
                            }
                        }
                    }
                    va_end(pp);
                }
                if (error == TGSERROR_OK) {
                    if (primary >= 0) {
                        for (i = 0; i < length; ++i) {
                            value += format[i][0];
                            value += (i == primary) ? (" like ") : ("=");
                            value += format[i][1];
                            value += ", ";
                        }
                        if ((error = print(value, &value, ap)) == TGSERROR_OK) {
                            if ((i = value.find(key)) != std::string::npos) {
                                value.erase(i, key.size() + 2);
                            }
                            if (value.size() >= 2) {
                                value.erase(value.size() - 2);
                            }
                            error = execute("UPDATE " + table + " SET " + value + " WHERE " + key + ((condition.empty()) ? ("") : (" " + condition)) + ";");
                        }
                    }
                    else {
                        for (i = 0; i < length; ++i) {
                            name += format[i][0];
                            name += ", ";
                            value += format[i][1];
                            value += ", ";
                        }
                        if ((error = print(value, &value, ap)) == TGSERROR_OK) {
                            if (name.size() >= 2) {
                                name.erase(name.size() - 2);
                            }
                            if (value.size() >= 2) {
                                value.erase(value.size() - 2);
                            }
                            error = execute("INSERT INTO " + table + " (" + name + ") VALUES (" + value + ");");
                        }
                    }
                }
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::remove(std::string const& table, char const* (*format)[2], va_ref<std::string const> condition, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, condition);
    error = remove(table, format, static_cast<std::string const&>(condition), ap);
    va_end(ap);
    return error;
}

/*protected */TGSError TGSDatabaseInterface::remove(std::string const& table, char const* (*format)[2], std::string const& condition, va_list ap)
{
    std::string string;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        if ((error = bind(format, &string, ap)) == TGSERROR_OK) {
            error = execute("DELETE FROM " + table + ((string.empty()) ? ("") : (" WHERE " + string)) + ((condition.empty()) ? ("") : (" " + condition)) + ";");
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::countRow(std::string const& table, char const* (*format)[2], std::string const& condition, int* result, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, result);
    error = countRow(table, format, condition, result, ap);
    va_end(ap);
    return error;
}

/*protected */TGSError TGSDatabaseInterface::countRow(std::string const& table, char const* (*format)[2], std::string const& condition, int* result, va_list ap)
{
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = select(table, "count(*)", format, condition, ap)) == TGSERROR_OK) {
            if ((error = step()) == TGSERROR_WAIT_RESULT) {
                if ((error = readInteger(0, result)) == TGSERROR_NO_RESULT) {
                    error = TGSERROR_OK;
                    *result = 0;
                }
                while (step() == TGSERROR_WAIT_RESULT);
            }
            else if (error == TGSERROR_OK) {
                error = TGSERROR_NO_RESULT;
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::select(std::string const& table, std::string const& query, char const* (*format)[2], va_ref<std::string const> condition, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, condition);
    error = select(table, query, format, static_cast<std::string const&>(condition), ap);
    va_end(ap);
    return error;
}

/*protected */TGSError TGSDatabaseInterface::select(std::string const& table, std::string const& query, char const* (*format)[2], std::string const& condition, va_list ap)
{
    std::string string;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        if ((error = bind(format, &string, ap)) == TGSERROR_OK) {
            error = prepare("SELECT " + ((query.empty()) ? ("*") : (query)) + " FROM " + table + ((string.empty()) ? ("") : (" WHERE " + string)) + ((condition.empty()) ? ("") : (" " + condition)) + ";");
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::execute(std::string const& sql)
{
    char* message;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        message = NULL;
        if (sqlite3_exec(_database, sql.c_str(), NULL, NULL, &message) != SQLITE_OK) {
            std::cerr << "database exec error [" << message << "]" << std::endl;
            error = TGSERROR_FAILED;
        }
        if (message != NULL) {
            sqlite3_free(message);
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::prepare(std::string const& sql)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        if (sqlite3_prepare_v2(_database, sql.c_str(), -1, &_statement, NULL) != SQLITE_OK) {
            std::cerr << "database prepare error [" << sqlite3_errmsg(_database) << "]" << std::endl;
            error = TGSERROR_FAILED;
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::step(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkStep()) == TGSERROR_OK) {
        switch (sqlite3_step(_statement)) {
            case SQLITE_DONE:
                // nop
                break;
            case SQLITE_ROW:
                error = TGSERROR_WAIT_RESULT;
                break;
            default:
                std::cerr << "database step error [" << sqlite3_errmsg(_database) << "]" << std::endl;
                error = TGSERROR_FAILED;
                break;
        }
        if (error != TGSERROR_WAIT_RESULT) {
            if (sqlite3_finalize(_statement) != SQLITE_OK) {
                std::cerr << "database finalize error [" << sqlite3_errmsg(_database) << "]" << std::endl;
            }
            _statement = NULL;
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::countColumn(int* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        *result = sqlite3_column_count(_statement);
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::getType(int column, int* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        *result = sqlite3_column_type(_statement, column);
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::getSize(int column, int* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        *result = sqlite3_column_bytes(_statement, column);
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::readInteger(int column, int* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        if (sqlite3_column_bytes(_statement, column) > 0) {
            *result = sqlite3_column_int(_statement, column);
        }
        else {
            error = TGSERROR_NO_RESULT;
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::readReal(int column, double* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        if (sqlite3_column_bytes(_statement, column) > 0) {
            *result = sqlite3_column_double(_statement, column);
        }
        else {
            error = TGSERROR_NO_RESULT;
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::readText(int column, std::string* result) const
{
    char const* temp;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkColumn(result)) == TGSERROR_OK) {
        if (sqlite3_column_bytes(_statement, column) > 0) {
            if ((temp = reinterpret_cast<char const*>(sqlite3_column_text(_statement, column))) != NULL) {
                *result = temp;
            }
            else {
                result->clear();
            }
        }
        else {
            error = TGSERROR_NO_RESULT;
        }
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::checkFlow(void) const
{
    TGSError error(TGSERROR_OK);
    
    if (_database != NULL) {
        if (_statement != NULL) {
            error = TGSERROR_INVALID_STATE;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::checkStep(void) const
{
    TGSError error(TGSERROR_OK);
    
    if (_database != NULL) {
        if (_statement == NULL) {
            error = TGSERROR_INVALID_STATE;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*protected */TGSError TGSDatabaseInterface::checkColumn(void* result) const
{
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        error = checkStep();
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected static */TGSError TGSDatabaseInterface::print(std::string const& format, std::string* result, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, result);
    error = print(format, result, ap);
    va_end(ap);
    return error;
}

/*protected static */TGSError TGSDatabaseInterface::print(std::string const& format, std::string* result, va_list ap)
{
    char* temp;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((temp = sqlite3_vmprintf(format.c_str(), ap)) != NULL) {
            *result = temp;
            sqlite3_free(temp);
        }
        else {
            error = TGSERROR_NO_MEMORY;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected static */TGSError TGSDatabaseInterface::buildQuery(std::string const& query, char const* (*format)[2], unsigned int length, std::string* result)
{
    std::string string;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        string = query;
        if (format != NULL) {
            for (i = 0; i < length; ++i) {
                if (!string.empty()) {
                    string += ", ";
                }
                string += format[i][0];
            }
        }
        *result = string;
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected static */TGSError TGSDatabaseInterface::buildOrder(std::string const& condition, char const* (*format)[2], bool order, int limit, int offset, std::string* result)
{
    std::string string;
    std::string temp;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        string = condition;
        if (format != NULL) {
            if (!string.empty()) {
                string += " ";
            }
            string += "ORDER BY ";
            string += (*format)[0];
            string += (order) ? (" ASC") : (" DESC");
        }
        if (limit >= 0) {
            if ((error = print("LIMIT %d", &temp, limit)) == TGSERROR_OK) {
                if (!string.empty()) {
                    string += " ";
                }
                string += temp;
                if (offset >= 0) {
                    if ((error = print("OFFSET %d", &temp, offset)) == TGSERROR_OK) {
                        string += " ";
                        string += temp;
                    }
                }
            }
        }
        if (error == TGSERROR_OK) {
            *result = string;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */TGSError TGSDatabaseInterface::bind(char const* (*format)[2], std::string* result, ...)
{
    va_list ap;
    TGSError error(TGSERROR_OK);
    
    va_start(ap, result);
    error = bind(format, result, ap);
    va_end(ap);
    return error;
}

/*private static */TGSError TGSDatabaseInterface::bind(char const* (*format)[2], std::string* result, va_list ap)
{
    std::string string;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if (format != NULL) {
            string += (*format)[0];
            string += " like ";
            string += (*format)[1];
        }
        if ((error = print(string, &string, ap)) == TGSERROR_OK) {
            *result = string;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

}// end of namespace
