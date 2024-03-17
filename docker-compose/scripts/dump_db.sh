#!/bin/bash
DUMPFILE="dump-$(date --iso-8601).pg"
DUMPFOLDER="/opt/victron-projekt/db_dumps/"
cd $DUMPFOLDER
docker exec -it victron-projekt_timescaledb_1 pg_dump victron >$DUMPFILE
gzip $DUMPFILE
cd -
