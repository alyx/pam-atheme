# PAM/Atheme

This module presents functionality for allowing your PAM system to integrate with your Atheme Services instance via XMLRPC.

## Required

1. PAM (obviously).
2. libxmlrpc / libxmlrpc-dev
3. An active Atheme instance with `modules/misc/httpd` and `modules/transport/xmlrpc` enabled.

## Config

Before compiling, you will want to edit `config.h` to update your network name and the endpoint for your Atheme XMLRPC instance.

## Usage

Simply add `pam-atheme.so` as an option to your desired chunk of PAM and wow! Instant Atheme!
