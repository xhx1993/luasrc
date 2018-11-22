/*
 ** $Id: lzio.c,v 1.31.1.1 2007/12/27 13:02:25 roberto Exp $
 ** a generic input stream interface
 ** See Copyright Notice in lua.h
 */


#include <string.h>

#define lzio_c
#define LUA_CORE

#include "lua.h"

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"


int luaZ_fill (ZIO *z) { //读取数据，放到z->p上，并且将n设置为未读的个数。Ps；fill会读取并返回一个字符
	size_t size;
	lua_State *L = z->L;
	const char *buff;
	lua_unlock(L);
	buff = z->reader(L, z->data, &size);
	lua_lock(L);
	if (buff == NULL || size == 0) return EOZ;
	z->n = size - 1;
	z->p = buff;
	return char2int(*(z->p++));
}


int luaZ_lookahead (ZIO *z) { //返回下一个字符，没有则返回EOZ  lookahead并不会修改到p和n
	if (z->n == 0) {
		if (luaZ_fill(z) == EOZ)
			return EOZ;
		else {
			z->n++;  /* luaZ_fill removed first byte; put back it */
			z->p--;
		}
	}
	return char2int(*z->p);
}


void luaZ_init (lua_State *L, ZIO *z, lua_Reader reader, void *data) {
	z->L = L;
	z->reader = reader;
	z->data = data;
	z->n = 0;
	z->p = NULL;
}


/* --------------------------------------------------------------- read --- */
size_t luaZ_read (ZIO *z, void *b, size_t n) {
	while (n) {
		size_t m;
		if (luaZ_lookahead(z) == EOZ)
			return n;  /* return number of missing bytes */
		m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
		memcpy(b, z->p, m);
		z->n -= m;
		z->p += m;
		b = (char *)b + m;
		n -= m;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */
char *luaZ_openspace (lua_State *L, Mbuffer *buff, size_t n) {
	if (n > buff->buffsize) {
		if (n < LUA_MINBUFFER) n = LUA_MINBUFFER;
		luaZ_resizebuffer(L, buff, n);
	}
	return buff->buffer;
}


