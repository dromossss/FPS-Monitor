#pragma once

#define XSTR_(macro) #macro
#define STRINGIFY_MACRO_CALL(macro) XSTR_(macro)
#define XLONG_(macro) L##macro
#define LONGIFY_MACRO_CALL(macro) XLONG_(macro)
#define SHM_S_(str) ShmString{ (str), charAlloc }

#define MAKE_MASTER_SYMBOL(enum_frag) PM_ENUM_##enum_frag
#define MAKE_ENUM_SYMBOL(enum_frag) PM_##enum_frag
#define CREATE_INTROSPECTION_ENUM(pSegmentManager, enum_frag, description) \
	ShmMakeUnique<IntrospectionEnum>(pSegmentManager, MAKE_MASTER_SYMBOL(enum_frag), SHM_S_(STRINGIFY_MACRO_CALL( \
	MAKE_ENUM_SYMBOL(enum_frag))), SHM_S_(description))
#define MAKE_LIST_SYMBOL(enum_frag) ENUM_KEY_LIST_##enum_frag
#define MAKE_KEY_SYMBOL(enum_frag, key_frag) PM_##enum_frag##_##key_frag
#define REGISTER_ENUM_KEY(pSegmentManager, p_enum_obj, enum_frag, key_frag, name, short_name, description) \
	p_enum_obj->AddKey(ShmMakeUnique<IntrospectionEnumKey>(pSegmentManager, MAKE_MASTER_SYMBOL(enum_frag), \
	MAKE_KEY_SYMBOL(enum_frag, key_frag), SHM_S_(STRINGIFY_MACRO_CALL(MAKE_KEY_SYMBOL(enum_frag, key_frag))), \
	SHM_S_(name), SHM_S_(short_name), SHM_S_(description)))