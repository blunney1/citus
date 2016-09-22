/*-------------------------------------------------------------------------
 *
 * metadata_cache.h
 *	  Executor support for Citus.
 *
 * Copyright (c) 2012-2016, Citus Data, Inc.
 *
 *-------------------------------------------------------------------------
 */

#ifndef METADATA_CACHE_H
#define METADATA_CACHE_H

#include "fmgr.h"
#include "distributed/master_metadata_utility.h"
#include "distributed/pg_dist_partition.h"
#include "distributed/worker_manager.h"
#include "utils/hsearch.h"


/*
 * Representation of a table's metadata that is frequently used for
 * distributed execution. Cached.
 */
typedef struct
{
	/* lookup key - must be first. A pg_class.oid oid. */
	Oid relationId;

	/*
	 * Has an invalidation been received for this entry, requiring a rebuild
	 * of the cache entry?
	 */
	bool isValid;

	bool isDistributedTable;
	bool hasUninitializedShardInterval;
	bool hasUniformHashDistribution; /* valid for hash partitioned tables */

	/* pg_dist_partition metadata for this table */
	char *partitionKeyString;
	char partitionMethod;

	/* pg_dist_shard metadata (variable-length ShardInterval array) for this table */
	int shardIntervalArrayLength;
	ShardInterval **sortedShardIntervalArray;

	FmgrInfo *shardIntervalCompareFunction; /* NULL if no shard intervals exist */
	FmgrInfo *hashFunction; /* NULL if table is not distributed by hash */
} DistTableCacheEntry;


extern bool IsDistributedTable(Oid relationId);
extern ShardInterval * LoadShardInterval(uint64 shardId);
extern DistTableCacheEntry * DistributedTableCacheEntry(Oid distributedRelationId);
extern void InsertNodeRow(int nodeid, char *nodename, int32 nodeport, uint32 groupId);
extern void CitusInvalidateRelcacheByRelid(Oid relationId);
extern void CitusInvalidateNodeCache(void);

extern bool CitusHasBeenLoaded(void);

/* access WorkerNodeHash */
extern HTAB * GetWorkerNodeHash(void);

/* relation oids */
extern Oid DistPartitionRelationId(void);
extern Oid DistShardRelationId(void);
extern Oid DistShardPlacementRelationId(void);
extern Oid DistNodeRelationId(void);

/* index oids */
extern Oid DistPartitionLogicalRelidIndexId(void);
extern Oid DistShardLogicalRelidIndexId(void);
extern Oid DistShardShardidIndexId(void);
extern Oid DistShardPlacementShardidIndexId(void);

/* function oids */
extern Oid CitusExtraDataContainerFuncId(void);

/* user related functions */
extern Oid CitusExtensionOwner(void);
extern char * CurrentUserName(void);
#endif /* METADATA_CACHE_H */
