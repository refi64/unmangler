from fbuild.builders.cxx import guess_static
from fbuild.config.c import cacheproperty
from fbuild.config.cxx import Test
from fbuild import ConfigFailed
from fbuild.db import caches

from optparse import make_option

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
        make_option('--use-color', help='Use colored output from the compiler',
                    action='store_true')
    ))

@caches
def configure(ctx):
    cxx = guess_static(ctx, platform_options=(
        ({'posix'}, {'flags+': ['-std=c++11']}),
        #({'clang++'}, {'flags+': ['-stdlib=libc++']}) # stupid libstdc++ bug
    ), exe=ctx.options.cxx, flags=(['-fdiagnostics-color'] if ctx.options.use_color\
                                  else [])+ctx.options.cxxflag)
    if not CheckBrokenRegex(cxx).broken_regex:
        raise ConfigFailed('Your libstdc++ version is too old; try a newer one '\
            'or pass --cxxflag=-stdlib=libc++ if you are using Clang and have '\
            'libc++ installed')
    return cxx

def build(ctx):
    cxx = configure(ctx)
    unmangler = cxx.build_exe('unmangler', ['unmangler.cpp'])
    ctx.install(unmangler, 'bin')
