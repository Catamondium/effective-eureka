#include <lua5.3/lua.h>
#include <lua5.3/lauxlib.h>
#include <lua5.3/lualib.h>

#include "vec.hpp"
#include "common.hpp"

static int tostring(lua_State *L);
static int index(lua_State *L);
static int newindex(lua_State *L);
static const struct luaL_Reg playerlib_m [] = {
    {"__index", index},
    {"__newindex", newindex},
    {"__tostring", tostring},
    {NULL, NULL}
};

struct Player {
    size_t score = 0;
    Vec pos = Vec{0, 0};

    /// Build Player closure
    void lua_serialize(lua_State *L)
    {
        Player *p = (Player *)lua_newuserdata(L, sizeof(Player));
        p->pos = pos;
        p->score = score;
        luaL_setmetatable(L, "player");

        lua_getglobal(L, "roam");
        lua_swap(L);
        lua_setfield(L, -2, "player");
    }

    void lua_refresh(lua_State *L)
    {
        lua_getglobal(L, "roam");
        lua_getfield(L, -1, "player");

        Player *p = (Player *)luaL_checkudata(L, -1, "player");
        pos.x = p->pos.x;
        pos.y = p->pos.y;
        score = p->score;
    }

    static void open(lua_State *L)
    {
        // player { __index = self }
        luaL_newmetatable(L, "player");
        lua_pushstring(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        //lua_newtable(L);
        assert(lua_istable(L, -1));
        luaL_setfuncs(L, playerlib_m, 0);
    }

    void move(Vec ds) {
        pos = pos + ds;
    }
};

static int tostring(lua_State *L)
{
    Player *p = (Player *)luaL_checkudata(L, 1, "player");
    lua_pushfstring(L, "Player(%d, pos{%d, %d})", p->score, p->pos.x, p->pos.y);
    return 1;
}

static int index(lua_State *L)
{
    Player *p = (Player *)luaL_checkudata(L, 1, "player");
    if (lua_isstring(L, 2)) {
        std::string key = std::string{luaL_checkstring(L, 2)};
        if (key == "pos") {
            p->pos.lua_serialize(L);
            return 1;
        } else if (key == "score") {
            lua_pushinteger(L, p->score);
            return 1;
        }

        for (auto method : playerlib_m) {
            if (key == method.name) {
                lua_pushcfunction(L, method.func);
                return 1;
            }
        }
    }
    return 0;
}

static int newindex(lua_State *L)
{
    Player *p = (Player *)luaL_checkudata(L, 1, "player");
    if (std::string{"pos"} == luaL_checkstring(L, 2)) {
        int x = p->pos.x;
        int y = p->pos.y;

        lua_getfield(L, 3, "x");
        if (lua_isinteger(L, -1)) {
            x = luaL_checkinteger(L, -1);
        }

        lua_getfield(L, 3, "y");
        if (lua_isinteger(L, -1)) {
            y = luaL_checkinteger(L, -1);
        }

        p->pos.x = x;
        p->pos.y = y;
    }
    return 0;
}
