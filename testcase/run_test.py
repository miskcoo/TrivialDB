#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import sys

for filename in os.listdir('.'):
    if filename.endswith('.sql'):
        f0 = filename[:-4]
        os.system('../build/trivial_db < %s 2> /dev/null > /dev/null' % filename)
        if not os.system('diff -ZB ans/%s.ans %s.out' % (f0, f0)):
            print('[Pass] %s' % filename)
        else: print('[Fail] %s' % filename)
        os.system('rm *.database *.thead *.tdata %s.out' % f0)
    elif filename.endswith('.py') and filename != 'run_test.py':
        f0 = filename[:-3]
        os.system('python3 ' + filename)
        os.system('../build/trivial_db < %s.sql 2> /dev/null > /dev/null' % f0)
        if not os.system('diff -ZB ans/%s.ans %s.out' % (f0, f0)):
            print('[Pass] %s' % filename)
        else: print('[Fail] %s' % filename)
        os.system('rm *.database *.thead *.tdata %s.sql %s.out ans/%s.ans' % (f0, f0, f0))
