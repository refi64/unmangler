from fbuild.builders.cxx import guess_static
from fbuild.db import caches

from optparse import make_option

def pre_options(parser):
    group = parser.add_option_group('config options')
    group.add_options((
        make_option('--cxx', help='Use the given C++ compiler'),
        make_option('--use-color', help='Use colored output from the compiler',
                    action='store_true')
    ))

@caches
def configure(ctx):
    cxx = guess_static(ctx, platform_options=(
        ({'posix'}, {'flags+': ['-std=c++11']}),
        ({'clang++'}, {'flags+': ['-stdlib=libc++']}) # stupid libstdc++ bug
    ), exe=ctx.options.cxx, flags=['-fcolor-diagnostics'] if ctx.options.use_color\
                                  else [])
    return cxx

def build(ctx):
    cxx = configure(ctx)
    unmangler = cxx.build_exe('unmangler', ['unmangler.cpp'])
    ctx.install(unmangler, 'bin')
