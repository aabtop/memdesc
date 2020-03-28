#ifndef MEMDESC_AST_SIZING_H_
#define MEMDESC_AST_SIZING_H_

#include "ast.h"

int AlignOf(const Struct& s);
int SizeOf(const Struct& s);
int OffsetOfField(const Struct& owning_struct, int field_index);

int AlignOf(const Type& type);
int SizeOf(const Type& type);

#endif  // MEMDESC_AST_SIZING_H_
