unmangler
=========

unmangler is a tool to take those ugly type names that ``typeid`` outputs (like ``St6vectorIS_ISt4pairIiPcESaIS2_EESaIS4_EE``) and turns it into color-coded type names.

See, ``c++filt`` also decodes type names, but the output looks kind of like this:

.. image:: http://s5.postimg.org/k5bigu7rb/cppfilt.png

That's more readable...a little...

With unmangler, that turns into:

.. image:: http://s17.postimg.org/ccoxk6uf3/unmangler_c.png

That's better!

Building
********

You need `Fbuild <https://github.com/felix-lang/fbuild/>`_ installed FROM GIT. Just run::
   
   $ fbuild

Installing
**********

::
   
   $ fbuild install

Usage
*****

Just run ``build/unmangler`` with the mangled name, and it will output a nice, color-coded representation, like::
   
   build/unmangler St6vectorIS_ISt4pairIiPcESaIS2_EESaIS4_EE
