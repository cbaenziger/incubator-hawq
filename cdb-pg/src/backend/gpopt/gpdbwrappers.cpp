//---------------------------------------------------------------------------
//	Greenplum Database
//	Copyright (C) 2012 EMC Corp.
//
//	@filename:
//		gpdbwrappers.cpp
//
//	@doc:
//		Implementation of GPDB function wrappers. Note that we should never
// 		return directly from inside the PG_TRY() block, in order to restore
//		the long jump stack. That is why we save the return value of the GPDB
//		function to a local variable and return it after the PG_END_TRY()
//
//	@owner:
//		elhela
//
//	@test:
//
//
//---------------------------------------------------------------------------

#define ALLOW_sigsetjmp
#define ALLOW_DatumGetBool
#define ALLOW_BoolGetDatum
#define ALLOW_DatumGetChar
#define ALLOW_CharGetDatum
#define ALLOW_DatumGetInt8
#define ALLOW_Int8GetDatum
#define ALLOW_DatumGetUInt8
#define ALLOW_UInt8GetDatum
#define ALLOW_DatumGetInt16
#define ALLOW_Int16GetDatum
#define ALLOW_DatumGetUInt16
#define ALLOW_UInt16GetDatum
#define ALLOW_DatumGetInt32
#define ALLOW_Int32GetDatum
#define ALLOW_DatumGetUInt32
#define ALLOW_UInt32GetDatum
#define ALLOW_DatumGetInt64
#define ALLOW_Int64GetDatum
#define ALLOW_DatumGetUInt64
#define ALLOW_UInt64GetDatum
#define ALLOW_DatumGetObjectId
#define ALLOW_DatumGetPointer
#define ALLOW_PointerGetDatum
#define ALLOW_aggregate_exists
#define ALLOW_bms_add_member
#define ALLOW_copyObject
#define ALLOW_datumGetSize
#define ALLOW_deconstruct_array
#define ALLOW_expression_tree_mutator
#define ALLOW_expression_tree_walker
#define ALLOW_exprType
#define ALLOW_exprTypmod
#define ALLOW_extract_nodes_plan
#define ALLOW_extract_nodes_expression
#define ALLOW_free_attstatsslot
#define ALLOW_func_strict
#define ALLOW_func_volatile
#define ALLOW_func_data_access
#define ALLOW_FuncnameGetCandidates
#define ALLOW_function_exists
#define ALLOW_function_oids
#define ALLOW_get_agg_transtype
#define ALLOW_get_aggregate
#define ALLOW_is_agg_ordered
#define ALLOW_get_array_type
#define ALLOW_get_attstatsslot
#define ALLOW_get_att_stats
#define ALLOW_get_commutator
#define ALLOW_trigger_exists
#define ALLOW_get_trigger_name
#define ALLOW_get_trigger_relid
#define ALLOW_get_trigger_funcid
#define ALLOW_get_trigger_type
#define ALLOW_trigger_enabled
#define ALLOW_get_func_name
#define ALLOW_get_func_output_arg_types
#define ALLOW_get_func_arg_types
#define ALLOW_get_func_retset
#define ALLOW_get_func_rettype
#define ALLOW_get_negator
#define ALLOW_get_opcode
#define ALLOW_get_opname
#define ALLOW_get_partition_attrs
#define ALLOW_rel_is_leaf_partition
#define ALLOW_rel_partition_get_master
#define ALLOW_BuildLogicalIndexInfo
#define ALLOW_get_parts
#define ALLOW_get_relation_keys
#define ALLOW_get_typ_typrelid
#define ALLOW_get_type_name
#define ALLOW_getgpsegmentCount
#define ALLOW_heap_attisnull
#define ALLOW_heap_freetuple
#define ALLOW_index_exists
#define ALLOW_isGreenplumDbHashable
#define ALLOW_lappend
#define ALLOW_lappend_int
#define ALLOW_lappend_oid
#define ALLOW_lcons
#define ALLOW_lcons_int
#define ALLOW_lcons_oid
#define ALLOW_list_concat
#define ALLOW_list_copy
#define ALLOW_list_head
#define ALLOW_list_tail
#define ALLOW_list_length
#define ALLOW_list_nth
#define ALLOW_list_free
#define ALLOW_list_nth_int
#define ALLOW_list_nth_oid
#define ALLOW_freeListAndNull
#define ALLOW_lookup_type_cache
#define ALLOW_makeString
#define ALLOW_makeBoolConst
#define ALLOW_makeTargetEntry
#define ALLOW_makeVar
#define ALLOW_MemoryContextAllocImpl
#define ALLOW_MemoryContextAllocZeroAlignedImpl
#define ALLOW_MemoryContextAllocZeroImpl
#define ALLOW_MemoryContextFreeImpl
#define ALLOW_MemoryContextReallocImpl
#define ALLOW_MemoryContextStrdup
#define ALLOW_nodeToString
#define ALLOW_numeric_to_double_no_overflow
#define ALLOW_op_hashjoinable
#define ALLOW_op_mergejoinable
#define ALLOW_op_input_types
#define ALLOW_operator_exists
#define ALLOW_palloc
#define ALLOW_pfree
#define ALLOW_pg_detoast_datum
#define ALLOW_query_or_expression_tree_walker
#define ALLOW_query_or_expression_tree_mutator
#define ALLOW_query_tree_mutator
#define ALLOW_range_table_mutator
#define ALLOW_rel_part_status
#define ALLOW_relation_exists
#define ALLOW_relation_oids
#define ALLOW_RelationClose
#define ALLOW_RelationGetIndexList
#define ALLOW_RelationBuildTriggers
#define ALLOW_RelationIdGetRelation
#define ALLOW_tlist_member
#define ALLOW_type_exists
#define ALLOW_GetExtTableEntry
#define ALLOW_ParseExternalTableUri
#define ALLOW_getCdbComponentDatabases
#define ALLOW_pg_strcasecmp
#define ALLOW_makeRandomSegMap
#define ALLOW_makeStringInfo
#define ALLOW_appendStringInfo
#define ALLOW_get_cast_func
#define ALLOW_get_relation_part_constraints
#define ALLOW_get_operator_type
#define ALLOW_get_comparison_operator
#define ALLOW_find_nodes
#define ALLOW_char_to_parttype
#define ALLOW_makeNullConst
#define ALLOW_coerce_to_common_type
#define ALLOW_resolve_generic_type
#define ALLOW_is_pxf_protocol

#include "gpopt/utils/gpdbdefs.h"

#include "gpos/base.h"
#include "gpos/error/CException.h"
#include "gpos/error/CAutoExceptionStack.h"

#include "exception.h"

#include "gpopt/gpdbwrappers.h"

#define GP_WRAP_START	\
	sigjmp_buf local_sigjmp_buf;	\
	{	\
		CAutoExceptionStack aes((void **) &PG_exception_stack, (void**) &error_context_stack);	\
		if (0 == sigsetjmp(local_sigjmp_buf, 0))	\
		{	\
			aes.SetLocalJmp(&local_sigjmp_buf)

#define GP_WRAP_END	\
		}	\
	}	\
	GPOS_RAISE(gpdxl::ExmaGPDB, gpdxl::ExmiGPDBError)

using namespace gpos;

bool
gpdb::FBoolFromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetBool(d);
	}
	GP_WRAP_END;
	return false;
}

Datum
gpdb::DDatumFromBool
	(
	bool b
	)
{
	GP_WRAP_START;
	{
		return BoolGetDatum(b);
	}
	GP_WRAP_END;
	return 0;
}

char
gpdb::CCharFromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetChar(d);
	}
	GP_WRAP_END;
	return '\0';
}

Datum
gpdb::DDatumFromChar
	(
	char c
	)
{
	GP_WRAP_START;
	{
		return CharGetDatum(c);
	}
	GP_WRAP_END;
	return 0;
}

int8
gpdb::CInt8FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetInt8(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromInt8
	(
	int8 i8
	)
{
	GP_WRAP_START;
	{
		return Int8GetDatum(i8);
	}
	GP_WRAP_END;
	return 0;
}

uint8
gpdb::UcUint8FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetUInt8(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromUint8
	(
	uint8 ui8
	)
{
	GP_WRAP_START;
	{
		return UInt8GetDatum(ui8);
	}
	GP_WRAP_END;
	return 0;
}

int16
gpdb::SInt16FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetInt16(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromInt16
	(
	int16 i16
	)
{
	GP_WRAP_START;
	{
		return Int16GetDatum(i16);
	}
	GP_WRAP_END;
	return 0;
}

uint16
gpdb::UsUint16FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetUInt16(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromUint16
	(
	uint16 ui16
	)
{
	GP_WRAP_START;
	{
		return UInt16GetDatum(ui16);
	}
	GP_WRAP_END;
	return 0;
}

int32
gpdb::IInt32FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetInt32(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromInt32
	(
	int32 i32
	)
{
	GP_WRAP_START;
	{
		return Int32GetDatum(i32);
	}
	GP_WRAP_END;
	return 0;
}

uint32
gpdb::UlUint32FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetUInt32(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromUint32
	(
	uint32 ui32
	)
{
	GP_WRAP_START;
	{
		return UInt32GetDatum(ui32);
	}
	GP_WRAP_END;
	return 0;
}

int64
gpdb::LlInt64FromDatum
	(
	Datum d
	)
{
	Datum d2 = d;
	GP_WRAP_START;
	{
		return DatumGetInt64(d2);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromInt64
	(
	int64 i64
	)
{
	int64 ii64 = i64;
	GP_WRAP_START;
	{
		return Int64GetDatum(ii64);
	}
	GP_WRAP_END;
	return 0;
}

uint64
gpdb::UllUint64FromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetUInt64(d);
	}
	GP_WRAP_END;
	return 0;
}

Datum
gpdb::DDatumFromUint64
	(
	uint64 ui64
	)
{
	GP_WRAP_START;
	{
		return UInt64GetDatum(ui64);
	}
	GP_WRAP_END;
	return 0;
}

Oid
gpdb::OidFromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetObjectId(d);
	}
	GP_WRAP_END;
	return 0;
}

void *
gpdb::PvPointerFromDatum
	(
	Datum d
	)
{
	GP_WRAP_START;
	{
		return DatumGetPointer(d);
	}
	GP_WRAP_END;
	return NULL;
}

Datum
gpdb::DDatumFromPointer
	(
	const void *p
	)
{
	GP_WRAP_START;
	{
		return PointerGetDatum(p);
	}
	GP_WRAP_END;
	return 0;
}

bool
gpdb::FAggregateExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return aggregate_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

Bitmapset *
gpdb::PbmsAddMember
	(
	Bitmapset *a,
	int x
	)
{
	GP_WRAP_START;
	{
		return bms_add_member(a, x);
	}
	GP_WRAP_END;
	return NULL;
}

void *
gpdb::PvCopyObject
	(
	void *from
	)
{
	GP_WRAP_START;
	{
		return copyObject(from);
	}
	GP_WRAP_END;
	return NULL;
}

Size
gpdb::SDatumSize
	(
	Datum value,
	bool typByVal,
	int iTypLen
	)
{
	GP_WRAP_START;
	{
		return datumGetSize(value, typByVal, iTypLen);
	}
	GP_WRAP_END;
	return 0;
}

void
gpdb::DeconstructArray
	(
	struct ArrayType *parray,
	Oid elmtype,
	int iElmlen,
	bool elmbyval,
	char cElmalign,
	Datum **ppElemSP,
	bool **nullsp,
	int *piElemSP
	)
{
	GP_WRAP_START;
	{
		deconstruct_array(parray, elmtype, iElmlen, elmbyval, cElmalign, ppElemSP, nullsp, piElemSP);
		return;
	}
	GP_WRAP_END;
}

Node *
gpdb::PnodeMutateExpressionTree
	(
	Node *pnode,
	Node *(*mutator) (),
	void *context
	)
{
	GP_WRAP_START;
	{
		return expression_tree_mutator(pnode, mutator, context);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FWalkExpressionTree
	(
	Node *pnode,
	bool (*walker) (),
	void *context
	)
{
	GP_WRAP_START;
	{
		return expression_tree_walker(pnode, walker, context);
	}
	GP_WRAP_END;
	return false;
}

Oid
gpdb::OidExprType
	(
	Node *pnodeExpr
	)
{
	GP_WRAP_START;
	{
		return exprType(pnodeExpr);
	}
	GP_WRAP_END;
	return 0;
}

int32
gpdb::IExprTypeMod
	(
	Node *pnodeExpr
	)
{
	GP_WRAP_START;
	{
		return exprTypmod(pnodeExpr);
	}
	GP_WRAP_END;
	return 0;
}

List *
gpdb::PlExtractNodesPlan
	(
	Plan *pl,
	int iNodeTag,
	bool descendIntoSubqueries
	)
{
	GP_WRAP_START;
	{
		return extract_nodes_plan(pl, iNodeTag, descendIntoSubqueries);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlExtractNodesExpression
	(
	Node *node,
	int iNodeTag
	)
{
	GP_WRAP_START;
	{
		return extract_nodes_expression(node, iNodeTag);
	}
	GP_WRAP_END;
	return NIL;
}

void
gpdb::FreeAttrStatsSlot
	(
	Oid atttype,
	Datum *pValues,
	int iValues,
	float4 *pNumbers,
	int iNumbers
	)
{
	GP_WRAP_START;
	{
		free_attstatsslot(atttype, pValues, iValues, pNumbers, iNumbers);
		return;
	}
	GP_WRAP_END;
}

bool
gpdb::FFuncStrict
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return func_strict(funcid);
	}
	GP_WRAP_END;
	return false;
}

char
gpdb::CFuncStability
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return func_volatile(funcid);
	}
	GP_WRAP_END;
	return '\0';
}

char
gpdb::CFuncDataAccess
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return func_data_access(funcid);
	}
	GP_WRAP_END;
	return '\0';
}

FuncCandidateList
gpdb::FclFuncCandidates
	(
	List *plistNames,
	int iArgs
	)
{
	GP_WRAP_START;
	{
		return FuncnameGetCandidates(plistNames, iArgs);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FFunctionExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return function_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

List *
gpdb::PlFunctionOids(void)
{
	GP_WRAP_START;
	{
		return function_oids();
	}
	GP_WRAP_END;
	return NIL;
}

Oid
gpdb::OidAggIntermediateResultType
	(
	Oid aggid
	)
{
	GP_WRAP_START;
	{
		return get_agg_transtype(aggid);
	}
	GP_WRAP_END;
	return 0;
}

bool
gpdb::FOrderedAgg
	(
	Oid aggid
	)
{
	GP_WRAP_START;
	{
		return is_agg_ordered(aggid);
	}
	GP_WRAP_END;
	return false;
}

Oid
gpdb::OidAggregate
	(
	const char *szAgg,
	Oid oidType
	)
{
	GP_WRAP_START;
	{
		return get_aggregate(szAgg, oidType);
	}
	GP_WRAP_END;
	return 0;
}

Oid
gpdb::OidArrayType
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return get_array_type(typid);
	}
	GP_WRAP_END;
	return 0;
}

bool
gpdb::FGetAttrStatsSlot
	(
	HeapTuple statstuple,
	Oid atttype,
	int32 atttypmod,
	int iReqKind,
	Oid reqop,
	Datum **ppValues,
	int *iValues,
	float4 **ppfNumbers,
	int *piNumbers
	)
{
	GP_WRAP_START;
	{
		return get_attstatsslot(statstuple, atttype, atttypmod, iReqKind, reqop, ppValues, iValues, ppfNumbers, piNumbers);
	}
	GP_WRAP_END;
	return false;
}

HeapTuple
gpdb::HtAttrStats
	(
	Oid relid,
	AttrNumber attnum
	)
{
	GP_WRAP_START;
	{
		return get_att_stats(relid, attnum);
	}
	GP_WRAP_END;
	return NULL;
}

Oid
gpdb::OidCommutatorOp
	(
	Oid opno
	)
{
	GP_WRAP_START;
	{
		return get_commutator(opno);
	}
	GP_WRAP_END;
	return 0;
}

char *
gpdb::SzTriggerName
	(
	Oid triggerid
	)
{
	GP_WRAP_START;
	{
		return get_trigger_name(triggerid);
	}
	GP_WRAP_END;
	return NULL;
}

Oid
gpdb::OidTriggerRelid
	(
	Oid triggerid
	)
{
	GP_WRAP_START;
	{
		return get_trigger_relid(triggerid);
	}
	GP_WRAP_END;
	return 0;
}

Oid
gpdb::OidTriggerFuncid
	(
	Oid triggerid
	)
{
	GP_WRAP_START;
	{
		return get_trigger_funcid(triggerid);
	}
	GP_WRAP_END;
	return 0;
}

int32
gpdb::ITriggerType
	(
	Oid triggerid
	)
{
	GP_WRAP_START;
	{
		return get_trigger_type(triggerid);
	}
	GP_WRAP_END;
	return 0;
}

bool
gpdb::FTriggerEnabled
	(
	Oid triggerid
	)
{
	GP_WRAP_START;
	{
		return trigger_enabled(triggerid);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FTriggerExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return trigger_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FCheckConstraintExists
	(
	Oid oidCheckConstraint
	)
{
	GP_WRAP_START;
	{
		return check_constraint_exists(oidCheckConstraint);
	}
	GP_WRAP_END;
	return false;
}

char *
gpdb::SzCheckConstraintName
	(
	Oid oidCheckConstraint
	)
{
	GP_WRAP_START;
	{
		return get_check_constraint_name(oidCheckConstraint);
	}
	GP_WRAP_END;
	return NULL;
}

Oid
gpdb::OidCheckConstraintRelid
	(
	Oid oidCheckConstraint
	)
{
	GP_WRAP_START;
	{
		return get_check_constraint_relid(oidCheckConstraint);
	}
	GP_WRAP_END;
	return 0;
}

Node *
gpdb::PnodeCheckConstraint
	(
	Oid oidCheckConstraint
	)
{
	GP_WRAP_START;
	{
		return get_check_constraint_expr_tree(oidCheckConstraint);
	}
	GP_WRAP_END;
	return NULL;
}

List *
gpdb::PlCheckConstraint
	(
	Oid oidRel
	)
{
	GP_WRAP_START;
	{
		return get_check_constraint_oids(oidRel);
	}
	GP_WRAP_END;
	return NULL;
}

Node *
gpdb::PnodePartConstraintRel
	(
	Oid oidRel,
	bool *pfDefaultConstraint
	)
{
	GP_WRAP_START;
	{
		return get_relation_part_constraints(oidRel, pfDefaultConstraint);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FLeafPartition
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return rel_is_leaf_partition(oid);
	}
	GP_WRAP_END;
	return false;
}

Oid
gpdb::OidRootPartition
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return rel_partition_get_master(oid);
	}
	GP_WRAP_END;
	return InvalidOid;
}

bool
gpdb::FCastFunc
	(
	Oid oidSrc,
	Oid oidDest, 
	bool *is_binary_coercible,
	Oid *oidCastFunc
	)
{
	GP_WRAP_START;
	{
		return get_cast_func(oidSrc, oidDest, is_binary_coercible, oidCastFunc);
	}
	GP_WRAP_END;
	return false;
}

uint
gpdb::UlCmpt
	(
	Oid oidOp,
	Oid oidLeft, 
	Oid oidRight
	)
{
	GP_WRAP_START;
	{
		return get_comparison_type(oidOp, oidLeft, oidRight);
	}
	GP_WRAP_END;
	return CmptOther;
}

Oid
gpdb::OidScCmp
	(
	Oid oidLeft, 
	Oid oidRight,
	uint ulCmpt
	)
{
	GP_WRAP_START;
	{
		return get_comparison_operator(oidLeft, oidRight, (CmpType) ulCmpt);
	}
	GP_WRAP_END;
	return InvalidOid;
}

char *
gpdb::SzFuncName
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return get_func_name(funcid);
	}
	GP_WRAP_END;
	return NULL;
}

List *
gpdb::PlFuncOutputArgTypes
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return get_func_output_arg_types(funcid);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlFuncArgTypes
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return get_func_arg_types(funcid);
	}
	GP_WRAP_END;
	return NIL;
}

bool
gpdb::FFuncRetset
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return get_func_retset(funcid);
	}
	GP_WRAP_END;
	return false;
}

Oid
gpdb::OidFuncRetType
	(
	Oid funcid
	)
{
	GP_WRAP_START;
	{
		return get_func_rettype(funcid);
	}
	GP_WRAP_END;
	return 0;
}

Oid
gpdb::OidInverseOp
	(
	Oid opno
	)
{
	GP_WRAP_START;
	{
		return get_negator(opno);
	}
	GP_WRAP_END;
	return 0;
}

RegProcedure
gpdb::OidOpFunc
	(
	Oid opno
	)
{
	GP_WRAP_START;
	{
		return get_opcode(opno);
	}
	GP_WRAP_END;
	return 0;
}

char *
gpdb::SzOpName
	(
	Oid opno
	)
{
	GP_WRAP_START;
	{
		return get_opname(opno);
	}
	GP_WRAP_END;
	return NULL;
}

List *
gpdb::PlPartitionAttrs
	(
	PartitionNode *pn
	)
{
	GP_WRAP_START;
	{
		return get_partition_attrs(pn);
	}
	GP_WRAP_END;
	return NIL;
}

PartitionNode *
gpdb::PpnParts
	(
	Oid relid,
	int2 level,
	Oid parent,
	bool inctemplate,
	MemoryContext mcxt
	)
{
	GP_WRAP_START;
	{
		return get_parts(relid, level, parent, inctemplate, mcxt);
	}
	GP_WRAP_END;
	return NULL;
}

List *
gpdb::PlRelationKeys
	(
	Oid relid
	)
{
	GP_WRAP_START;
	{
		return get_relation_keys(relid);
	}
	GP_WRAP_END;
	return NIL;
}

Oid
gpdb::OidTypeRelid
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return get_typ_typrelid(typid);
	}
	GP_WRAP_END;
	return 0;
}

char *
gpdb::SzTypeName
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return get_type_name(typid);
	}
	GP_WRAP_END;
	return NULL;
}

int
gpdb::UlSegmentCountGP(void)
{
	GP_WRAP_START;
	{
		return getgpsegmentCount();
	}
	GP_WRAP_END;
	return 0;
}

bool
gpdb::FHeapAttIsNull
	(
	HeapTuple tup,
	int iAttNum
	)
{
	GP_WRAP_START;
	{
		return heap_attisnull(tup, iAttNum);
	}
	GP_WRAP_END;
	return false;
}

void
gpdb::FreeHeapTuple
	(
	HeapTuple htup
	)
{
	GP_WRAP_START;
	{
		heap_freetuple(htup);
		return;
	}
	GP_WRAP_END;
}

bool
gpdb::FIndexExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return index_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FGreenplumDbHashable
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return isGreenplumDbHashable(typid);
	}
	GP_WRAP_END;
	return false;
}

List *
gpdb::PlAppendElement
	(
	List *plist,
	void *datum
	)
{
	GP_WRAP_START;
	{
		return lappend(plist, datum);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlAppendInt
	(
	List *plist,
	int iDatum
	)
{
	GP_WRAP_START;
	{
		return lappend_int(plist, iDatum);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlAppendOid
	(
	List *plist,
	Oid datum
	)
{
	GP_WRAP_START;
	{
		return lappend_oid(plist, datum);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlPrependElement
	(
	void *datum,
	List *list
	)
{
	GP_WRAP_START;
	{
		return lcons(datum, list);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlPrependInt
	(
	int datum,
	List *list
	)
{
	GP_WRAP_START;
	{
		return lcons_int(datum, list);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlPrependOid
	(
	Oid datum,
	List *list
	)
{
	GP_WRAP_START;
	{
		return lcons_oid(datum, list);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlConcat
	(
	List *list1,
	List *list2
	)
{
	GP_WRAP_START;
	{
		return list_concat(list1, list2);
	}
	GP_WRAP_END;
	return NIL;
}

List *
gpdb::PlCopy
	(
	List *list
	)
{
	GP_WRAP_START;
	{
		return list_copy(list);
	}
	GP_WRAP_END;
	return NIL;
}

ListCell *
gpdb::PlcListHead
	(
	List *l
	)
{
	GP_WRAP_START;
	{
		return list_head(l);
	}
	GP_WRAP_END;
	return NULL;
}

ListCell *
gpdb::PlcListTail
	(
	List *l
	)
{
	GP_WRAP_START;
	{
		return list_tail(l);
	}
	GP_WRAP_END;
	return NULL;
}

int
gpdb::UlListLength
	(
	List *l
	)
{
	GP_WRAP_START;
	{
		return list_length(l);
	}
	GP_WRAP_END;
	return 0;
}

void *
gpdb::PvListNth
	(
	List *list,
	int n
	)
{
	GP_WRAP_START;
	{
		return list_nth(list, n);
	}
	GP_WRAP_END;
	return NULL;
}

int
gpdb::IListNth
	(
	List *list,
	int n
	)
{
	GP_WRAP_START;
	{
		return list_nth_int(list, n);
	}
	GP_WRAP_END;
	return 0;
}


Oid
gpdb::OidListNth
	(
	List *list,
	int n
	)
{
	GP_WRAP_START;
	{
		return list_nth_oid(list, n);
	}
	GP_WRAP_END;
	return 0;
}

void
gpdb::FreeList
	(
	List *plist
	)
{
	GP_WRAP_START;
	{
		list_free(plist);
		return;
	}
	GP_WRAP_END;
}

void
gpdb::FreeListAndNull
	(
	List **listPtrPtr
	)
{
	GP_WRAP_START;
	{
		freeListAndNull(listPtrPtr);
		return;
	}
	GP_WRAP_END;
}

TypeCacheEntry *
gpdb::PtceLookup
	(
	Oid type_id,
	int flags
	)
{
	GP_WRAP_START;
	{
		return lookup_type_cache(type_id, flags);
	}
	GP_WRAP_END;
	return NULL;
}

Value *
gpdb::PvalMakeString
	(
	char *str
	)
{
	GP_WRAP_START;
	{
		return makeString(str);
	}
	GP_WRAP_END;
	return NULL;
}

Node *
gpdb::PnodeMakeBoolConst
	(
	bool value,
	bool isnull
	)
{
	GP_WRAP_START;
	{
		return makeBoolConst(value, isnull);
	}
	GP_WRAP_END;
	return NULL;
}

Node *
gpdb::PnodeMakeNULLConst
	(
	Oid oidType
	)
{
	GP_WRAP_START;
	{
		return (Node *) makeNullConst(oidType, -1 /*consttypmod*/);
	}
	GP_WRAP_END;
	return NULL;
}

TargetEntry *
gpdb::PteMakeTargetEntry
	(
	Expr *pnodeExpr,
	AttrNumber resno,
	char *resname,
	bool resjunk
	)
{
	GP_WRAP_START;
	{
		return makeTargetEntry(pnodeExpr, resno, resname, resjunk);
	}
	GP_WRAP_END;
	return NULL;
}

Var *
gpdb::PvarMakeVar
	(
	Index varno,
	AttrNumber varattno,
	Oid vartype,
	int32 vartypmod,
	Index varlevelsup
	)
{
	GP_WRAP_START;
	{
		return makeVar(varno, varattno, vartype, vartypmod, varlevelsup);
	}
	GP_WRAP_END;
	return NULL;
}

void *
gpdb::PvMemoryContextAllocImpl
	(
	MemoryContext context,
	Size size,
	const char* file,
	const char * func,
	int line
	)
{
	GP_WRAP_START;
	{
		return MemoryContextAllocImpl(context, size, file, func, line);
	}
	GP_WRAP_END;
	return NULL;
}

void *
gpdb::PvMemoryContextAllocZeroAlignedImpl
	(
	MemoryContext context,
	Size size,
	const char* file,
	const char * func,
	int line
	)
{
	GP_WRAP_START;
	{
		return MemoryContextAllocZeroAlignedImpl(context, size, file, func, line);
	}
	GP_WRAP_END;
	return NULL;
}

void *
gpdb::PvMemoryContextAllocZeroImpl
	(
	MemoryContext context,
	Size size,
	const char* file,
	const char * func,
	int line
	)
{
	GP_WRAP_START;
	{
		return MemoryContextAllocZeroImpl(context, size, file, func, line);
	}
	GP_WRAP_END;
	return NULL;
}

void *
gpdb::PvMemoryContextReallocImpl
	(
	void *pointer,
	Size size,
	const char* file,
	const char * func,
	int line
	)
{
	GP_WRAP_START;
	{
		return MemoryContextReallocImpl(pointer, size, file, func, line);
	}
	GP_WRAP_END;
	return NULL;
}

char *
gpdb::SzMemoryContextStrdup
	(
	MemoryContext context,
	const char *string
	)
{
	GP_WRAP_START;
	{
		return MemoryContextStrdup(context, string);
	}
	GP_WRAP_END;
	return NULL;
}

char *
gpdb::SzNodeToString
	(
	void *obj
	)
{
	GP_WRAP_START;
	{
		return nodeToString(obj);
	}
	GP_WRAP_END;
	return NULL;
}

Node *
gpdb::Pnode
	(
	char *string
	)
{
	GP_WRAP_START;
	{
		return (Node*) stringToNode(string);
	}
	GP_WRAP_END;
	return NULL;
}


Node *
gpdb::PnodeTypeDefault
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return get_typdefault(typid);
	}
	GP_WRAP_END;
	return NULL;
}


double
gpdb::DNumericToDoubleNoOverflow
	(
	Numeric num
	)
{
	GP_WRAP_START;
	{
		return numeric_to_double_no_overflow(num);
	}
	GP_WRAP_END;
	return 0.0;
}

bool
gpdb::FOpHashJoinable
	(
	Oid opno
	)
{
	GP_WRAP_START;
	{
		return op_hashjoinable(opno);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FOpMergeJoinable
	(
	Oid opno,
	Oid *leftOp,
	Oid *rightOp
	)
{
	GP_WRAP_START;
	{
		return op_mergejoinable(opno, leftOp, rightOp);
	}
	GP_WRAP_END;
	return false;
}

void
gpdb::GetOpInputTypes
	(
	Oid opno,
	Oid *lefttype,
	Oid *righttype
	)
{
	GP_WRAP_START;
	{
		op_input_types(opno, lefttype, righttype);
		return;
	}
	GP_WRAP_END;
}

bool
gpdb::FOperatorExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return operator_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

void *
gpdb::GPDBAlloc
	(
	Size size
	)
{
	GP_WRAP_START;
	{
		return palloc(size);
	}
	GP_WRAP_END;
	return NULL;
}

void
gpdb::GPDBFree
	(
	void *ptr
	)
{
	GP_WRAP_START;
	{
		pfree(ptr);
		return;
	}
	GP_WRAP_END;
}

struct varlena *
gpdb::PvlenDetoastDatum
	(
	struct varlena * datum
	)
{
	GP_WRAP_START;
	{
		return pg_detoast_datum(datum);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FWalkQueryOrExpressionTree
	(
	Node *pnode,
	bool (*walker) (),
	void *context,
	int flags
	)
{
	GP_WRAP_START;
	{
		return query_or_expression_tree_walker(pnode, walker, context, flags);
	}
	GP_WRAP_END;
	return false;
}

Node *
gpdb::PnodeMutateQueryOrExpressionTree
	(
	Node *pnode,
	Node *(*mutator) (),
	void *context,
	int flags
	)
{
	GP_WRAP_START;
	{
		return query_or_expression_tree_mutator(pnode, mutator, context, flags);
	}
	GP_WRAP_END;
	return NULL;
}

Query *
gpdb::PqueryMutateQueryTree
	(
	Query *query,
	Node *(*mutator) (),
	void *context,
	int flags
	)
{
	GP_WRAP_START;
	{
		return query_tree_mutator(query, mutator, context, flags);
	}
	GP_WRAP_END;
	return NULL;
}

List *
gpdb::PlMutateRangeTable
	(
	List *rtable,
	Node *(*mutator) (),
	void *context,
	int flags
	)
{
	GP_WRAP_START;
	{
		return range_table_mutator(rtable, mutator, context, flags);
	}
	GP_WRAP_END;
	return NIL;
}

bool
gpdb::FRelPartIsRoot
	(
	Oid relid
	)
{
	GP_WRAP_START;
	{
		return PART_STATUS_ROOT == rel_part_status(relid);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FHashPartitioned
	(
	char c
	)
{
	GP_WRAP_START;
	{
		return PARTTYP_HASH == char_to_parttype(c);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FRelationExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return relation_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

List *
gpdb::PlRelationOids(void)
{
	GP_WRAP_START;
	{
		return relation_oids();
	}
	GP_WRAP_END;
	return NIL;
}

void
gpdb::CloseRelation
	(
	Relation rel
	)
{
	GP_WRAP_START;
	{
		RelationClose(rel);
		return;
	}
	GP_WRAP_END;
}

List *
gpdb::PlRelationIndexes
	(
	Relation relation
	)
{
	GP_WRAP_START;
	{
		return RelationGetIndexList(relation);
	}
	GP_WRAP_END;
	return NIL;
}

LogicalIndexes *
gpdb::Plgidx
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return BuildLogicalIndexInfo(oid);
	}
	GP_WRAP_END;
	return NULL;
}

void
gpdb::BuildRelationTriggers
	(
	Relation rel
	)
{
	GP_WRAP_START;
	{
		RelationBuildTriggers(rel);
		return;
	}
	GP_WRAP_END;
}

Relation
gpdb::RelGetRelation
	(
	Oid relationId
	)
{
	GP_WRAP_START;
	{
		return RelationIdGetRelation(relationId);
	}
	GP_WRAP_END;
	return NULL;
}

ExtTableEntry *
gpdb::Pexttable
	(
	Oid relationId
	)
{
	GP_WRAP_START;
	{
		return GetExtTableEntry(relationId);
	}
	GP_WRAP_END;
	return NULL;
}

TargetEntry *
gpdb::PteMember
	(
	Node *pnode,
	List *targetlist
	)
{
	GP_WRAP_START;
	{
		return tlist_member(pnode, targetlist);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FTypeExists
	(
	Oid oid
	)
{
	GP_WRAP_START;
	{
		return type_exists(oid);
	}
	GP_WRAP_END;
	return false;
}

bool
gpdb::FCompositeType
	(
	Oid typid
	)
{
	GP_WRAP_START;
	{
		return type_is_rowtype(typid);
	}
	GP_WRAP_END;
	return false;
}

int
gpdb::IValue
	(
	Node *pnode
	)
{
	GP_WRAP_START;
	{
		return intVal(pnode);
	}
	GP_WRAP_END;
	return 0;
}

Uri *
gpdb::PuriParseExternalTable
	(
	const char *szUri
	)
{
	GP_WRAP_START;
	{
		return ParseExternalTableUri(szUri);
	}
	GP_WRAP_END;
	return NULL;
}

bool
gpdb::FPxfProtocol
	(
	Uri *pUri
	)
{
	GP_WRAP_START;
	{
		return is_pxf_protocol(pUri);
	}
	GP_WRAP_END;
	return false;
}

CdbComponentDatabases *
gpdb::PcdbComponentDatabases(void)
{
	GP_WRAP_START;
	{
		return getCdbComponentDatabases();
	}
	GP_WRAP_END;
	return NULL;
}

int
gpdb::IStrCmpIgnoreCase
	(
	const char *sz1,
	const char *sz2
	)
{
	GP_WRAP_START;
	{
		return pg_strcasecmp(sz1, sz2);
	}
	GP_WRAP_END;
	return 0;
}

bool *
gpdb::RgfRandomSegMap
	(
	int total_primaries,
	int total_to_skip
	)
{
	GP_WRAP_START;
	{
		return makeRandomSegMap(total_primaries, total_to_skip);
	}
	GP_WRAP_END;
	return NULL;
}

StringInfo
gpdb::SiMakeStringInfo(void)
{
	GP_WRAP_START;
	{
		return makeStringInfo();
	}
	GP_WRAP_END;
	return NULL;
}

void
gpdb::AppendStringInfo
	(
	StringInfo str,
	const char *str1,
	const char *str2
	)
{
	GP_WRAP_START;
	{
		appendStringInfo(str, "%s%s", str1, str2);
		return;
	}
	GP_WRAP_END;
}

int
gpdb::IFindNodes
	(
	Node *node,
	List *nodeTags
	)
{
	GP_WRAP_START;
	{
		return find_nodes(node, nodeTags);
	}
	GP_WRAP_END;
	return -1;
}

Node *
gpdb::PnodeCoerceToCommonType
	(
	ParseState *pstate,
	Node *pnode,
	Oid oidTargetType,
	const char *context
	)
{
	GP_WRAP_START;
	{
		return coerce_to_common_type
					(
					pstate,
					pnode,
					oidTargetType,
					context
					);
	}
	GP_WRAP_END;
	return NULL;
}

Oid
gpdb::OidResolveGenericType
	(
	Oid declared_type,
	Oid context_actual_type,
	Oid context_declared_type
	)
{
	GP_WRAP_START;
	{
		return resolve_generic_type(declared_type, context_actual_type, context_declared_type);
	}
	GP_WRAP_END;
	return 0;
}

// EOF