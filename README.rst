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

                set resp.http.hello = signature.valid_signature(<"plain message">, <"signature in Base64 encoded">, <"rsa public key">);

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
                set resp.http.Var_Public_Key = {"-----BEGIN RSA PUBLIC KEY-----
        MIGJAoGBAMtTkwOx7XYzs4rNn9pNDzWtDIJkS/hwHaQnYlG39d4B941TO4rbIYUl
        6NGGG+H3ObRIUNtR5FRId0PIZmy5fuyBJSeY73b1jWmLeq7Ht5EPjOTzxOYTYCCA
        L+tAxNDFPcbr5OYYc324OCYV5oq4cYhH4jBYECnCzlRsvocf8cczAgMBAAE=
        -----END RSA PUBLIC KEY-----"};

                # This sets resp.http.is_valid to true or false
                set resp.http.is_valid = signature.valid_signature(resp.http.Var_Message, resp.http.Var_Signature, resp.http.Var_Public_Key);
        }

**NOTE:**
 The public key is formatting in the PEM RSAPublicKey. Not formatting in the PEM(`ref <https://openssl.org/docs/manmaster/apps/rsa.html>`_).

COMMON PROBLEMS
===============

* configure: error: Need varnish.m4 -- see README.rst

  Check if ``PKG_CONFIG_PATH`` has been set correctly before calling
  ``autogen.sh`` and ``configure``
