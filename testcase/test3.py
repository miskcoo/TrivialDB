#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import random

create_stmt = '''
CREATE DATABASE db_test3;
SET OUTPUT = 'test3.out';
USE db_test3;
CREATE TABLE Table (
    ID int,
    Key varchar(5000)
);
'''

def gen_item():
    return (random.randint(1, 10000), '%s' % random.randint(1, 10 ** 4999))

random.seed(123)
fout = open('test3.sql', 'w')
fans = open('ans/test3.ans', 'w')

fout.write(create_stmt)
A = [ gen_item() for i in range(5000) ]

fout.write('INSERT INTO Table VALUES ' + ','.join([ "(%d, '%s')" % (x[0], x[1]) for x in A ]) + ';\n')

A.reverse()
def prefix_select(prefix):
    fout.write('SELECT Key FROM Table WHERE Key LIKE \'%s' % prefix)
    fout.write('%\';\n')
    fans.write('Key\n')
    for _, k in A:
        if k.startswith(prefix):
            fans.write(k + '\n')
    fans.write('\n')

prefix_select('123')
prefix_select('12323')
for i in range(20):
    prefix_select('%d' % i)
prefix_select('49')
prefix_select(A[4][1][:100])

fout.close()
fans.close()
