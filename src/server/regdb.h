/**
* regdb.h
*
* Init Created: 2014-08-29
* Last Updated: 2016-07-15
*/
#ifndef REGDB_H_INCLUDED
#define REGDB_H_INCLUDED

#include "server_conf.h"


static char regdb_cresql[] =
    "CREATE TABLE IF NOT EXISTS reg(\n"
    "    clientid VARCHAR(60) NOT NULL PRIMARY KEY,\n"
    "    ipaddr VARCHAR(39) NOT NULL,\n"
    "    hostname VARCHAR(60),\n"
    "    macaddr VARCHAR(17),\n"
    "    pkgver VARCHAR(30),\n"
    "    pkgsum VARCHAR(32),\n"
    "    confsum VARCHAR(32),\n"
    "    regtime INTEGER DEFAULT 0,\n"
    "    lastdt TIMESTAMP DEFAULT (DATETIME('now','localtime')),\n"
    "    evtlog VARCHAR(200)\n"
    ");\n"
    "CREATE INDEX IF NOT EXISTS idx_reg ON reg(ipaddr,hostname,macaddr);";


static int regdb_open (const char *dbprefix, sqlite3 **pdb)
{
    int rc;
    char dbfile[FSYNC_PATHPREFIX_LEN + 10];

    sqlite3 *db = 0;
    *pdb = 0;

    /**
     * open regdb, if not exist, create it
     */
    sprintf(dbfile, "%s/.regdb", dbprefix);
    LOGGER_INFO("open regdb: %s", dbfile);

    rc = sqlite3_open_v2(dbfile, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX, 0);

    if (rc == SQLITE_CANTOPEN) {
        LOGGER_INFO("create regdb: %s", dbfile);
        rc = sqlite3_open_v2(dbfile, &db,
            SQLITE_OPEN_READWRITE|SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_CREATE, 0);
    }

    if (rc == SQLITE_OK) {
        char * errmsg  = 0;

        LOGGER_INFO("init regdb: %s", dbfile);
        rc = sqlite3_exec(db, regdb_cresql, 0, 0, &errmsg);

        if (rc != SQLITE_OK) {
            LOGGER_FATAL("sqlite3_exec error(%d): %s", rc, errmsg);
            sqlite3_free(errmsg);
            sqlite3_close_v2(db);
            return ERROR;
        }
    } else {
        LOGGER_FATAL("sqlite3_open_v2 error(%d): %s", rc, sqlite3_errstr(rc));
        return ERROR;
    }

    *pdb = db;
    return SUCCESS;
}


static void regdb_close (sqlite3 **pdb)
{
    sqlite3 *db = *pdb;
    if (db) {
        sqlite3_close_v2(db);
        *pdb = 0;
    }
}



#endif /* REGDB_H_INCLUDED */
