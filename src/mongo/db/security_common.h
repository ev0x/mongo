// security_common.h

/**
*    Copyright (C) 2009 10gen Inc.
*    Copyright (C) 2013 Tokutek Inc.
*
*    This program is free software: you can redistribute it and/or  modify
*    it under the terms of the GNU Affero General Public License, version 3,
*    as published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "commands.h"
#include "../util/concurrency/spin_lock.h"

namespace mongo {

    /**
     * Internal secret key info.
     */
    struct AuthInfo {
        AuthInfo() {
            user = "__system";
        }
        string user;
        string pwd;
    };

    // --noauth cmd line option
    extern bool noauth;
    extern AuthInfo internalSecurity; // set at startup and not changed after initialization.

    /**
     * This method checks the validity of filename as a security key, hashes its
     * contents, and stores it in the internalSecurity variable.  Prints an
     * error message to the logs if there's an error.
     * @param filename the file containing the key
     * @return if the key was successfully stored
     */
    bool setUpSecurityKey(const string& filename);

    class CmdAuthenticate : public InformationCommand {
    public:
        CmdAuthenticate() : InformationCommand("authenticate") {}
        virtual bool requiresAuth() { return false; }
        virtual void help(stringstream& ss) const { ss << "internal"; }
        bool run(const string& dbname , BSONObj& cmdObj, int options, string& errmsg, BSONObjBuilder& result, bool fromRepl);
        void authenticate(const string& dbname, const string& user, const bool readOnly);
    private:
        bool getUserObj(const string& dbname, const string& user, BSONObj& userObj, string& pwd);
    };
    
    extern CmdAuthenticate cmdAuthenticate;

    class CmdLogout : public InformationCommand {
    public:
        CmdLogout() : InformationCommand("logout") {}
        void help(stringstream& h) const { h << "de-authenticate"; }
        bool run(const string& dbname , BSONObj& cmdObj, int options, string& errmsg, BSONObjBuilder& result, bool fromRepl);
    };

} // namespace mongo