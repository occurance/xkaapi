changequote(`[', `]')dnl
dnl
dnl These macros come from autoconf/m4sugar/m4sugar.m4
dnl The goal is to use foreach_w (foreach for whitespace separated list)
dnl and append_uniq_w
dnl
define([flatten],
[ifelse(index([$1], [
]), [-1], [[$1]],
       [translit(patsubst([[[$1]]], [\\
]), [
], [ ])])])dnl
define([_split],
[changequote([-=<{(],[)}>=-])]dnl
[[patsubst(-=<{(-=<{($1)}>=-)}>=-, -=<{($2)}>=-,
               -=<{(]$3[)}>=-)]changequote([, ])])dnl
define([map_args_w],
[_$0(_split([ ]flatten([$1])[ ], [[	 ]+],
               ifelse(index([$2$3$4], [\]), [-1], [[$3[]$4[]$2]],
                     [patsubst([[$3[]$4[]$2]], [\\], [\\\\])])),
     len([[]$3[]$4]), len([$4[]$2[]]))])dnl
define([_map_args_w],
[substr([$1], [$2], eval(len([$1]) - [$2] - [$3]))])dnl
define([foreach_w],
[pushdef([$1])map_args_w([$2],
  [define([$1],], [)$3])popdef([$1])])dnl
define([append],
[define([$1], ifdef([$1], [defn([$1])[$3]])[$2])])dnl
define([_append_uniq],
[ifdef([$1],
          [ifelse(index([$3]defn([$1])[$3], [$3$2$3]), [-1],
                 [append([$1], [$2], [$3])$4], [$5])],
          [define([$1], [$2])$4])])dnl
define([append_uniq_w],
[map_args_w([$2], [_append_uniq([$1],], [, [ ])])])dnl
dnl
dnl End of macros from autoconf/m4sugar/m4sugar.m4
dnl
define([uniq_w],
[pushdef([LIST])undefine([LIST])dnl
foreach_w(VAR,$1,[append_uniq_w([LIST],VAR)])dnl
ifdef([LIST],LIST,[])
popdef([LIST])dnl
])dnl
define([GET],[dnl
ifdef([$1_$2],[$1_$2],[])])dnl
define([PKGCONFIG_VARS],[dnl
foreach_w(VAR,uniq_w($1), [dnl
[PKGCONFIG_]VAR[_CFLAGS]=$(shell $(PKG_CONFIG) --cflags VAR)
[PKGCONFIG_]VAR[_LIBS]=$(shell $(PKG_CONFIG) --libs VAR)
])])dnl
define([C_OBJS],
[foreach_w([SRC],$2,[patsubst(SRC,[^\(.*/\)?\([^/]+\).c$],[\2.o ])])])dnl
define([CXX_OBJS],
[foreach_w([SRC],$2,[patsubst(SRC,[^\(.*/\)?\([^/]+\).cpp$],[\2.o ])])])dnl
define([C_RULES],
[C_OBJS([$1],[$2]): $2
	$(CC) $(CFLAGS) GET([$1],[CFLAGS]) foreach_w([L],GET([$1],[PKGLIBS]),[$(PKGCONFIG_[]L[]_CFLAGS)]) $(CPPFLAGS) GET([$1],[CPPFLAGS]) -MMD -MF .$[]@.d -c $(OUTPUT_OPTION) $<
])dnl
define([CXX_RULES],
[CXX_OBJS([$1],[$2]): $2
	$(CXX) $(CXXFLAGS) GET([$1],[CXXFLAGS]) foreach_w([L],GET([$1],[PKGLIBS]),[$(PKGCONFIG_[]L[]_CFLAGS)]) $(CPPFLAGS) GET([$1],[CPPFLAGS]) -MMD -MF .$[]@.d -c $(OUTPUT_OPTION) $<
])dnl
define([PROG_RULES],
[define([$1_C_SOURCES],
    [foreach_w([SRC],$1_SOURCES_REAL,[ifelse(regexp(SRC,[^.*\.c$]),[-1],[],[ SRC])])])dnl
define([$1_CXX_SOURCES],
    [foreach_w([SRC],$1_SOURCES_REAL,[ifelse(regexp(SRC,[^.*\.cpp$]),[-1],[],[ SRC])])])dnl
define([$1_OBJS],
    [CXX_OBJS([$1],[$1_CXX_SOURCES])C_OBJS([$1],[$1_C_SOURCES])])dnl

###################
# $1
foreach_w([C_SOURCE],$1_C_SOURCES,[C_RULES([$1], [C_SOURCE])])
foreach_w([CXX_SOURCE],$1_CXX_SOURCES,[CXX_RULES([$1], [CXX_SOURCE])])
$1: $1_OBJS
	LINKER([$1]) $(LDFLAGS) $^ foreach_w([L],GET([$1],[PKGLIBS]),[$(PKGCONFIG_[]L[]_LIBS)]) GET([$1],[LDADD]) $(LDLIBS) -o $[]@
clean::
	$(RM) $1_OBJS[]foreach_w([OBJ],$1_OBJS,[ [.]OBJ[.d]])

-include[]foreach_w([OBJ],$1_OBJS,[ [.]OBJ[.d]])
])dnl
define([LINKER],[ifelse($1_CXX_SOURCES,[],[$(CC)],[$(CXX)])])dnl
define([_CLEANUP_SOURCES],dnl
[dnl
define($1_REAL,
	[foreach_w([SRC],$1,[ifelse(substr(SRC,0,len(DIR)),[DIR],[../SRC],[substr(SRC,incr(len(DIR))) ])])])dnl
])dnl
define([CLEANUP_SOURCES],
[foreach_w([PROG],PROGS,[_CLEANUP_SOURCES(PROG[_SOURCES])])])dnl
define([Makefile],[dnl
# Autogenerated example Makefile
# This Makefile needed xkaapi to be already installed system-wide
# (ie pkg-config invokations must work)

[PROGS]=PROGS

CPPFLAGS=-Wall
CFLAGS=-O2
CXXFLAGS=-O2

CLEANUP_SOURCES[]dnl
PKGCONFIG_VARS([foreach_w([PROG],PROGS,[GET(PROG,[PKGLIBS]) ])])

[all: $(PROGS)]

foreach_w([PROG],PROGS,[PROG_RULES(PROG)])
])dnl