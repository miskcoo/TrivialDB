#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import sys
import time

for filename in list(os.listdir('.')):
    if filename.endswith('.sql'):
        f0 = filename[:-4]
        t_start = time.time()
        os.system('../build/trivial_db < %s 2> /dev/null > /dev/null' % filename)
        if not os.system('diff -ZB ans/%s.ans %s.out' % (f0, f0)):
            print('[Pass] %s, elapsed = %.2lfs' % (filename, time.time() - t_start))
        else:
            print('[Fail] %s, elapsed = %.2lfs' % (filename, time.time() - t_start))
            break
        os.system('rm *.database *.thead *.tdata %s.out' % f0)
    elif filename.endswith('.py') and filename != 'run_test.py':
        f0 = filename[:-3]
        os.system('python3 ' + filename)
        t_start = time.time()
        os.system('../build/trivial_db < %s.sql 2> /dev/null > /dev/null' % f0)
        if not os.system('diff -ZB ans/%s.ans %s.out' % (f0, f0)):
            print('[Pass] %s, elapsed = %.2lfs' % (filename, time.time() - t_start))
        else:
            print('[Fail] %s, elapsed = %.2lfs' % (filename, time.time() - t_start))
            break
        os.system('rm *.database *.thead *.tdata %s.sql %s.out ans/%s.ans' % (f0, f0, f0))
