#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import random

create_stmt = '''
CREATE DATABASE db_test4;
SET OUTPUT = 'test4.out';
USE db_test4;
CREATE TABLE Table (
    ID int,
    Key varchar(20)
);
CREATE INDEX Table(ID);
CREATE INDEX Table(Key);
'''

def gen_item():
    return (random.randint(1, 10000), '%s' % random.randint(1, 10 ** 20))

random.seed(123)
fout = open('test4.sql', 'w')
fans = open('ans/test4.ans', 'w')

fout.write(create_stmt)
A = [ gen_item() for i in range(10000000) ]

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

for i in range(10):
    prefix_select('%d%d' % (i, i))

def count_select(num):
    fout.write('SELECT COUNT(*) FROM Table WHERE ID = %d;' % num)
    fans.write('COUNT(*)\n')
    count = 0
    for i, _ in A:
        if i == num:
            count += 1
    fans.write('%d\n\n' % count)

for i in range(200):
    count_select(i)

fout.close()
fans.close()
