unmangler
=========

unmangler is a tool to take those ugly type names that ``typeid`` outputs (like ``St6vectorIS_ISt4pairIiPcESaIS2_EESaIS4_EE``) and turns it into color-coded type names.

See, ``c++filt`` also decodes type names, but the output looks kind of like this:

.. image:: http://s5.postimg.org/k5bigu7rb/cppfilt.png

That's more readable...a little...

With unmangler, that turns into:

.. image:: http://s5.postimg.org/lyef55sxz/unmangler.png

That's better!

Building
********

You need `pconfigure <https://github.com/palmer-dabbelt/pconfigure/>`_ installed. If you get weird errors where make complains about ``cxxabi.h`` being nonexistent, try `this fork <https://github.com/kirbyfan64/pconfigure>`_. Just run::
   
   $ pconfigure

Followed by::
   
   $ make

Usage
*****

Just run ``bin/unmangler`` with the mangled name, and it will output a nice, color-coded representation, like::
   
   bin/unmangler St6vectorIS_ISt4pairIiPcESaIS2_EESaIS4_EE

You can also run ``make install`` to install unmangler.
