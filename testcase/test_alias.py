#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import random

create_stmt = '''
CREATE DATABASE db_test_alias;
SET OUTPUT = 'test_alias.out';
USE db_test_alias;
CREATE TABLE Table (
    ID int,
    Key varchar(5)
);
CREATE INDEX Table(Key);
'''

def gen_item():
    return (random.randint(1, 10000), '%s' % random.randint(1, 99))

random.seed(123)
fout = open('test_alias.sql', 'w')
fans = open('ans/test_alias.ans', 'w')

fout.write(create_stmt)
A = [ gen_item() for i in range(1000) ]

fout.write('INSERT INTO Table VALUES ' + ','.join([ "(%d, '%s')" % (x[0], x[1]) for x in A ]) + ';\n')
fout.write('SELECT COUNT(*) FROM Table AS A, Table AS B WHERE A.Key = B.Key;')
fout.write('SELECT COUNT(*) FROM Table AS A, Table AS B, Table AS C WHERE A.Key = B.Key AND A.Key = C.Key;')
fout.write('SELECT COUNT(*) FROM Table AS A, Table AS B, Table AS C, Table AS D WHERE A.Key = B.Key AND A.Key = C.Key AND D.Key = C.Key;')
fout.write('SELECT COUNT(*) FROM Table AS A, Table AS B, Table AS C WHERE A.Key = B.Key AND A.Key = \'23\' AND A.Key = C.Key;')

Ac = [0] * 100
for i in A:
    Ac[int(i[1])] += 1

C2, C3, C4 = 0, 0, 0
for a in Ac:
    C2 += a ** 2
    C3 += a ** 3
    C4 += a ** 4

fans.write('COUNT(*)\n%d\n' % C2)
fans.write('COUNT(*)\n%d\n' % C3)
fans.write('COUNT(*)\n%d\n' % C4)
fans.write('COUNT(*)\n%d\n' % Ac[23] ** 3)

fout.close()
fans.close()
