from fbuild.config.c import cacheproperty, header_test
from fbuild.builders.cxx import guess_static
from fbuild.config.cxx import Test
from fbuild.record import Record
from fbuild import ConfigFailed
from fbuild.db import caches

from optparse import make_option

class RE2(Test):
    re2_h = header_test('re2/re2.h')

class CheckBrokenRegex(Test):
    @cacheproperty
    def broken_regex(self):
        return self.builder.check_run('''
            #include <string>
            #include <regex>

            using namespace std;

            int main() {
                return !regex_search(string{"int"}, regex{"int"});
            }
            ''', 'checking for a broken C++11 regex implementation')

def pre_options(parser):
    group = parser.add_option_group('config options')
    group.add_options((
        make_option('--cxx', help='Use the given C++ compiler'),
        make_option('--cxxflag', help='Pass the given flag to the C++ compiler',
                    action='append', default=[]),
        make_option('--no-re2', help="Don't use RE2, even if it's installed",
                    action='store_true'),
        make_option('--use-color', help='Use colored output from the compiler',
                    action='store_true')
    ))

@caches
def configure(ctx):
    cxx = guess_static(ctx, platform_options=(
        ({'posix'}, {'flags+': ['-std=c++11']}),
    ), exe=ctx.options.cxx, flags=(['-fdiagnostics-color'] if ctx.options.use_color\
                                  else [])+ctx.options.cxxflag)
    libs = []
    defs = []
    if not ctx.options.no_re2 and RE2(cxx).re2_h:
        libs.append('re2')
        defs.append('USE_RE2')
    else:
        if not CheckBrokenRegex(cxx).broken_regex:
            raise ConfigFailed('Your libstdc++ version is too old; try a newer '\
                'one, pass --cxxflag=-stdlib=libc++ if you are using Clang and '\
                'have libc++ installed, or install RE2')
    return Record(cxx=cxx, libs=libs, defs=defs)

def build(ctx):
    rec = configure(ctx)
    cxx = rec.cxx
    libs = rec.libs
    defs = rec.defs
    unmangler = cxx.build_exe('unmangler', ['unmangler.cpp'], external_libs=libs,
                              macros=defs)
    ctx.install(unmangler, 'bin')
