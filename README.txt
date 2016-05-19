Installation:
As for installation you need the following prerequisites: 

mbedTLS - Can be compiled from the attached source code or using one of the methods described in the link: https://libraries.io/homebrew/mbedtl/mbedtls-1.3.12 (for Mac users: brew install mbedtls)

Bitpunch - enter the Bitpunch directory and type: make shared-lib (a shared library will be created)

Both directories (Bitpunch and Cryptobox) must be placed together in a system stracture (f.e. in a directory due to relative paths in makefile). Bitpunch requires also libtasn1, for more detail visit: https://github.com/FrUh/BitPunch/

Cryptobox - enter the cryptobox directory and type make

