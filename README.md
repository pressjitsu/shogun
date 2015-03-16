Dynamic PHP Malware Analysis Toolkit
====================================

# Codename: Shogun

This application sanitized and runs PHP code to reveal misbehaving features that are typical of malware. It outputs results that can be parsed by other scripts, which can do fancy stuff like decide whether it's really malware or not based on heuristics against the pierced code.

## How it works

Nobody knows. It tries to safely execute parts of PHP code to arrive to hidden branches, obfuscated function calls, etc. to reveal their true intent.

## Roadmap

Right now we're testing the output of each opcode against all possible opcodes http://php.net/manual/en/internals2.opcodes.php and fixing output issues against a minimal test set.

# Installation

Compiles as a PHP SAPI module. Place into `sapi/shogun`, run `autoconf -f` to force autoconf rebuild to run, configure `--enable-shogun` and `make -j$(nproc) shogun` as usual.

# Usage

`./shogun shell.php`

# Issues

Segmentation faults. DoS in production, should not be trusted so sanitize your environment (read: sandbox virtual machine).

# License

GPLv3 http://www.gnu.org/copyleft/gpl.html
Copyright (c) Pressjitsu
