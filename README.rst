============
vmod_signature
============

----------------------
Varnish Signature Module
----------------------

:Date: 2016-02-24
:Version: 1.0
:Manual section: 3

SYNOPSIS
========

import signature;

DESCRIPTION
===========

This Varnish vmod verifies the signature with the specific message and public key.

FUNCTIONS
=========

valid_signature
-----

Prototype
        ::

                valid_signature(STRING msg, STRING sig, STRING key)
Return value
	BOOL
Description
	Returns TRUE if verification of the signature is succeeded, otherwise FALSE.
Example
        ::

                set resp.http.is_valid = signature.valid_signature(<"plain message">, <"signature in Base64 encoded">, <"public key">);

INSTALLATION
============

The source tree is based on autotools to configure the building, and
does also have the necessary bits in place to do functional unit tests
using the ``varnishtest`` tool.

Building requires the Varnish header files and uses pkg-config to find
the necessary paths.

Usage::

 ./autogen.sh
 ./configure

If you have installed Varnish to a non-standard directory, call
``autogen.sh`` and ``configure`` with ``PKG_CONFIG_PATH`` pointing to
the appropriate path. For signature, when varnishd configure was called
with ``--prefix=$PREFIX``, use

 PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
 export PKG_CONFIG_PATH

Make targets:

* make - builds the vmod.
* make install - installs your vmod.
* make check - runs the unit tests in ``src/tests/*.vtc``
* make distcheck - run check and prepare a tarball of the vmod.

In your VCL you could then use this vmod along the following lines::

        import signature;

        sub vcl_deliver {
                set resp.http.Var_Message = "吾輩は猫である";
                set resp.http.Var_Signature = "I2NjSC5IM3o6II54M5Ock4pfyl0hTR1bclPNJQ+x8j9Bx8Rncz1pZZy98GP4wtSINLXFogmCQFVHok/8/PSDVLTP5x1VexovUizWjJsntfO534i1WoUyS03ArqaTwZEXV7CHCSmHu9OruBRHoPKc2iic+SEaUz8NuT2gqzuv8ZI=";
                set resp.http.Var_Public_Key = {"-----BEGIN PUBLIC KEY-----
        MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDLU5MDse12M7OKzZ/aTQ81rQyC
        ZEv4cB2kJ2JRt/XeAfeNUzuK2yGFJejRhhvh9zm0SFDbUeRUSHdDyGZsuX7sgSUn
        mO929Y1pi3qux7eRD4zk88TmE2AggC/rQMTQxT3G6+TmGHN9uDgmFeaKuHGIR+Iw
        WBApws5UbL6HH/HHMwIDAQAB
        -----END PUBLIC KEY-----"};

                # This sets resp.http.is_valid to true or false
                set resp.http.is_valid = signature.valid_signature(resp.http.Var_Message, resp.http.Var_Signature, resp.http.Var_Public_Key);
        }

COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling
  ``autogen.sh`` and ``configure``
