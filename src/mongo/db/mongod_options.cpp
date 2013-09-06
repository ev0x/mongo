/*
 *    Copyright (C) 2013 10gen Inc.
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
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the GNU Affero General Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/db/mongod_options.h"

#include <string>
#include <vector>

#include "mongo/base/status.h"
#include "mongo/bson/util/builder.h"
#include "mongo/db/server_options.h"
#include "mongo/util/net/ssl_options.h"
#include "mongo/util/options_parser/option_description.h"
#include "mongo/util/options_parser/option_section.h"

namespace mongo {

    typedef moe::OptionDescription OD;
    typedef moe::PositionalOptionDescription POD;

    extern std::string dbpath;

    Status addMongodOptions(moe::OptionSection* options) {

        moe::OptionSection general_options("General options");

        Status ret = addGeneralServerOptions(&general_options);
        if (!ret.isOK()) {
            return ret;
        }

#if defined(_WIN32)
        moe::OptionSection windows_scm_options("Windows Service Control Manager options");

        ret = addWindowsServerOptions(&windows_scm_options);
        if (!ret.isOK()) {
            return ret;
        }
#endif

#ifdef MONGO_SSL
        moe::OptionSection ssl_options("SSL options");

        ret = addSSLServerOptions(&ssl_options);
        if (!ret.isOK()) {
            return ret;
        }
#endif

        moe::OptionSection ms_options("Master/slave options (old; use replica sets instead)");
        moe::OptionSection rs_options("Replica set options");
        moe::OptionSection replication_options("Replication options");
        moe::OptionSection sharding_options("Sharding options");

        StringBuilder dbpathBuilder;
        dbpathBuilder << "directory for datafiles - defaults to " << dbpath;

        ret = general_options.addOption(OD("auth", "auth", moe::Switch, "run with security", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("cpu", "cpu", moe::Switch,
                    "periodically show cpu and iowait utilization", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("dbpath", "dbpath", moe::String,
                    dbpathBuilder.str().c_str(), true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("diaglog", "diaglog", moe::Int,
                    "0=off 1=W 2=R 3=both 7=W+some reads", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("directoryperdb", "directoryperdb", moe::Switch,
                    "each database will be stored in a separate directory", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("ipv6", "ipv6", moe::Switch,
                    "enable IPv6 support (disabled by default)", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("journal", "journal", moe::Switch, "enable journaling",
                    true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("journalCommitInterval", "journalCommitInterval",
                    moe::Unsigned, "how often to group/batch commit (ms)", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("journalOptions", "journalOptions", moe::Int,
                    "journal diagnostic options", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("jsonp", "jsonp", moe::Switch,
                    "allow JSONP access via http (has security implications)", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("noauth", "noauth", moe::Switch, "run without security",
                    true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("noIndexBuildRetry", "noIndexBuildRetry", moe::Switch,
                    "don't retry any index builds that were interrupted by shutdown", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("nojournal", "nojournal", moe::Switch,
                    "disable journaling (journaling is on by default for 64 bit)", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("noprealloc", "noprealloc", moe::Switch,
                    "disable data file preallocation - will often hurt performance", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("noscripting", "noscripting", moe::Switch,
                    "disable scripting engine", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("notablescan", "notablescan", moe::Switch,
                    "do not allow table scans", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("nssize", "nssize", moe::Int,
                    ".ns file size (in MB) for new databases", true, moe::Value(16)));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("profile", "profile", moe::Int, "0=off 1=slow, 2=all",
                    true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("quota", "quota", moe::Switch,
                    "limits each database to a certain number of files (8 default)", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("quotaFiles", "quotaFiles", moe::Int,
                    "number of files allowed per db, requires --quota", true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("repair", "repair", moe::Switch, "run repair on all dbs",
                    true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("repairpath", "repairpath", moe::String,
                    "root directory for repair files - defaults to dbpath" , true));
        if (!ret.isOK()) {
            return ret;
        }
        ret = general_options.addOption(OD("rest", "rest", moe::Switch, "turn on simple rest api",
                    true));
        if (!ret.isOK()) {
            return ret;
        }
#if defined(__linux__)
        general_options.addOptionChaining("shutdown", "shutdown", moe::Switch,
                "kill a running server (for init scripts)");

        general_options.addOptionChaining("dur", "dur", moe::Switch, "enable journaling")
                                         .hidden()
                                         .setSources(moe::SourceAllLegacy);

        general_options.addOptionChaining("nodur", "nodur", moe::Switch, "disable journaling")
                                         .hidden()
                                         .setSources(moe::SourceAllLegacy);

        // Way to enable or disable journaling in JSON Config
        general_options.addOptionChaining("storage.journal.enabled", "", moe::Bool,
                "enable journaling")
                                         .setSources(moe::SourceYAMLConfig);

        // Two ways to set durability diagnostic options.  durOptions is deprecated
        general_options.addOptionChaining("storage.journal.debugFlags", "journalOptions", moe::Int,
                "journal diagnostic options")
                                         .incompatibleWith("durOptions");

        general_options.addOptionChaining("durOptions", "durOptions", moe::Int,
                "durability diagnostic options")
                                         .hidden()
                                         .setSources(moe::SourceAllLegacy)
                                         .incompatibleWith("storage.journal.debugFlags");

        general_options.addOptionChaining("storage.journal.commitIntervalMs",
                "journalCommitInterval", moe::Unsigned, "how often to group/batch commit (ms)");

        // Deprecated option that we don't want people to use for performance reasons
        options->addOptionChaining("nopreallocj", "nopreallocj", moe::Switch,
                "don't preallocate journal files")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

#if defined(__linux__)
        general_options.addOptionChaining("shutdown", "shutdown", moe::Switch,
                "kill a running server (for init scripts)");

#endif

        // Master Slave Options

        ms_options.addOptionChaining("master", "master", moe::Switch, "master mode")
                                    .setSources(moe::SourceAllLegacy);

        ms_options.addOptionChaining("slave", "slave", moe::Switch, "slave mode")
                                    .setSources(moe::SourceAllLegacy);

        ms_options.addOptionChaining("source", "source", moe::String,
                "when slave: specify master as <server:port>")
                                    .setSources(moe::SourceAllLegacy);

        ms_options.addOptionChaining("only", "only", moe::String,
                "when slave: specify a single database to replicate")
                                    .setSources(moe::SourceAllLegacy);

        ms_options.addOptionChaining("slavedelay", "slavedelay", moe::Int,
                "specify delay (in seconds) to be used when applying master ops to slave")
                                    .setSources(moe::SourceAllLegacy);

        ms_options.addOptionChaining("autoresync", "autoresync", moe::Switch,
                "automatically resync if slave data is stale")
                                    .setSources(moe::SourceAllLegacy);

        // Replication Options

        replication_options.addOptionChaining("replication.oplogSizeMB", "oplogSize", moe::Int,
                "size to use (in MB) for replication op log. default is 5% of disk space "
                "(i.e. large is good)");

        rs_options.addOptionChaining("replication.replSet", "replSet", moe::String,
                "arg is <setname>[/<optionalseedhostlist>]")
                                    .setSources(moe::SourceAllLegacy)
                                    .incompatibleWith("replication.replSetName");

        rs_options.addOptionChaining("replication.replSetName", "", moe::String, "arg is <setname>")
                                    .setSources(moe::SourceYAMLConfig)
                                    .format("[^/]", "[replica set name with no \"/\"]")
                                    .incompatibleWith("replication.replSet");

        rs_options.addOptionChaining("replication.secondaryIndexPrefetch", "replIndexPrefetch", moe::String,
                "specify index prefetching behavior (if secondary) [none|_id_only|all]")
                                    .format("(:?none)|(:?_id_only)|(:?all)",
                                            "(none/_id_only/all)");

        // Sharding Options

        sharding_options.addOptionChaining("sharding.configsvr", "configsvr", moe::Switch,
                "declare this is a config db of a cluster; default port 27019; "
                "default dir /data/configdb")
                                          .setSources(moe::SourceAllLegacy)
                                          .incompatibleWith("sharding.clusterRole");

        sharding_options.addOptionChaining("sharding.shardsvr", "shardsvr", moe::Switch,
                "declare this is a shard db of a cluster; default port 27018")
                                          .setSources(moe::SourceAllLegacy)
                                          .incompatibleWith("sharding.clusterRole");

        sharding_options.addOptionChaining("sharding.clusterRole", "", moe::String,
                "Choose what role this mongod has in a sharded cluster.  Possible values are:\n"
                "    \"configsvr\": Start this node as a config server.  Starts on port 27019 by "
                "default."
                "    \"shardsvr\": Start this node as a shard server.  Starts on port 27018 by "
                "default.")
                                          .setSources(moe::SourceYAMLConfig)
                                          .incompatibleWith("sharding.configsvr")
                                          .incompatibleWith("sharding.shardsvr")
                                          .format("(:?configsvr)|(:?shardsvr)",
                                                  "(configsvr/shardsvr)");

        sharding_options.addOptionChaining("sharding.noMoveParanoia", "noMoveParanoia", moe::Switch,
                "turn off paranoid saving of data for the moveChunk command; default")
                                          .hidden()
                                          .setSources(moe::SourceAllLegacy);

        sharding_options.addOptionChaining("sharding.archiveMovedChunks", "moveParanoia",
                moe::Switch, "turn on paranoid saving of data during the moveChunk command "
                "(used for internal system diagnostics)")
                                          .hidden();

        options->addSection(general_options);
#if defined(_WIN32)
        options->addSection(windows_scm_options);
#endif
        options->addSection(replication_options);
        options->addSection(ms_options);
        options->addSection(rs_options);
        options->addSection(sharding_options);
#ifdef MONGO_SSL
        options->addSection(ssl_options);
#endif

        // The following are legacy options that are disallowed in the JSON config file

        options->addOptionChaining("fastsync", "fastsync", moe::Switch,
                "indicate that this instance is starting from a dbpath snapshot of the repl peer")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        options->addOptionChaining("pretouch", "pretouch", moe::Int,
                "n pretouch threads for applying master/slave operations")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        // This is a deprecated option that we are supporting for backwards compatibility
        // The first value for this option can be either 'dbpath' or 'run'.
        // If it is 'dbpath', mongod prints the dbpath and exits.  Any extra values are ignored.
        // If it is 'run', mongod runs normally.  Providing extra values is an error.
        options->addOptionChaining("command", "command", moe::StringVector, "command")
                                  .hidden()
                                  .positional(1, 3)
                                  .setSources(moe::SourceAllLegacy);

        options->addOptionChaining("cacheSize", "cacheSize", moe::Long,
                "cache size (in MB) for rec store")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        // things we don't want people to use
        options->addOptionChaining("nohints", "nohints", moe::Switch, "ignore query hints")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        // deprecated pairing command line options
        options->addOptionChaining("pairwith", "pairwith", moe::Switch, "DEPRECATED")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        options->addOptionChaining("arbiter", "arbiter", moe::Switch, "DEPRECATED")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        options->addOptionChaining("opIdMem", "opIdMem", moe::Switch, "DEPRECATED")
                                  .hidden()
                                  .setSources(moe::SourceAllLegacy);

        return Status::OK();
    }

} // namespace mongo
