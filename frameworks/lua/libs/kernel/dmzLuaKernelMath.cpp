#include <dmzLuaKernel.h>
#include "dmzLuaKernelPrivate.h"
#include <dmzLuaKernelValidate.h>
#include <dmzTypesConsts.h>
#include <dmzTypesVector.h>

#include <luacpp.h>

using namespace dmz;

namespace {

static int
math_is_zero (lua_State *L) {

   const Float64 Value = luaL_checknumber (L, 1);
   const Float64 Epsilon = luaL_optnumber (L, 2, dmz::Epsilon64);

   lua_pushboolean (L, is_zero64 (Value, Epsilon) ? 1 : 0);

   return 1;
}


static int
math_to_degrees (lua_State *L) {

   const Float64 Value = luaL_checknumber (L, 1);

   lua_pushnumber (L, to_degrees (Value));

   return 1;
}


static int
math_to_radians (lua_State *L) {

   const Float64 Value = luaL_checknumber (L, 1);

   lua_pushnumber (L, to_radians (Value));

   return 1;
}


static int
math_up (lua_State *L) {

   const Vector Value (0.0, 1.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
math_down (lua_State *L) {

   const Vector Value (0.0, -1.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
math_right (lua_State *L) {

   const Vector Value (1.0, 0.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
math_left (lua_State *L) {

   const Vector Value (-1.0, 0.0, 0.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}


static int
math_forward (lua_State *L) {

   const Vector Value (0.0, 0.0, -1.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}



static int
math_backward (lua_State *L) {

   const Vector Value (0.0, 0.0, 1.0);
   return lua_create_vector (L, &Value) ? 1 : 0;
}



static const luaL_Reg arrayFunc [] = {
   {"is_zero", math_is_zero},
   {"to_degrees", math_to_degrees},
   {"to_radians", math_to_radians},
   {"up", math_up},
   {"down", math_down},
   {"right", math_right},
   {"left", math_left},
   {"forward", math_forward},
   {"backward", math_backward},
   {NULL, NULL},
};

};


//! \cond
void
dmz::open_lua_kernel_math_lib (lua_State *L) {

   LUA_START_VALIDATE (L);

   lua_create_dmz_namespace (L, "math");
   const int Table = lua_gettop (L);
   luaL_register (L, NULL, arrayFunc);

   lua_pushliteral (L, "Pi");
   lua_pushnumber (L, Pi64);
   lua_rawset (L, Table);

   lua_pushliteral (L, "TwoPi");
   lua_pushnumber (L, TwoPi64);
   lua_rawset (L, Table);

   lua_pushliteral (L, "HalfPi");
   lua_pushnumber (L, HalfPi64);
   lua_rawset (L, Table);

   lua_pushliteral (L, "Epsilon");
   lua_pushnumber (L, Epsilon64);
   lua_rawset (L, Table);

   lua_make_readonly (L, -1); // make runtime read only.
   lua_pop (L, 1); // pops meta table and dmz.runtime table.

   LUA_END_VALIDATE (L, 0);
}
//! \endcond

