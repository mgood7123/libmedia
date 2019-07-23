//
// Created by konek on 7/14/2019.
//

#ifndef GLES3JNI_FUNCTIONPOINTER_H
#define GLES3JNI_FUNCTIONPOINTER_H
#define functionPointerDeclare0(type, name) type (*name)()
#define functionPointerDeclare1(type, name, type1) type (*name)(type1)
#define functionPointerDeclare2(type, name, type1, type2) type (*name)(type1, type2)
#define functionPointerDeclare4(type, name, type1, type2, type3, type4) type (*name)(type1, type2, type3, type4)
#define functionPointerCast0(type) type (*)()
#define functionPointerCast1(type, type1) type (*)(type1)
#define functionPointerCast2(type, type1, type2) type (*)(type1, type2)
#define functionPointerCast4(type, type1, type2, type3, type4) type (*)(type1, type2, type3, type4)
#define functionPointerAssign0(type, f, what) f = (functionPointerCast0(type)) what
#define functionPointerAssign1(type, f, what, type1) f = (functionPointerCast1(type, type1)) what
#define functionPointerAssign2(type, f, what, type1, type2) f = (functionPointerCast2(type, type1, type2)) what
#define functionPointerAssign4(type, f, what, type1, type2, type3, type4) f = (functionPointerCast4(type, type1, type2, type3, type4)) what
#endif //GLES3JNI_FUNCTIONPOINTER_H
