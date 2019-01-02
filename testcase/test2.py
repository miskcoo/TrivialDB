#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import random

create_stmt = '''
CREATE DATABASE db_test2;
SET OUTPUT = 'test2.out';
USE db_test2;
CREATE TABLE Table1 (
    ID int,
    Key varchar(5)
);

CREATE TABLE Table2 (
    ID int,
    Key varchar(5)
);

CREATE TABLE Table3 (
    ID int,
    Key varchar(5)
);
CREATE INDEX Table1(Key);
CREATE INDEX Table2(Key);
'''

def gen_item():
    return (random.randint(1, 10000), '%s' % random.randint(1, 99))

random.seed(123)
fout = open('test2.sql', 'w')
fans = open('ans/test2.ans', 'w')

fout.write(create_stmt)
A = [ gen_item() for i in range(30000) ]
B = [ gen_item() for i in range(1000) ]
C = [ gen_item() for i in range(50) ]

fout.write('INSERT INTO Table1 VALUES ' + ','.join([ "(%d, '%s')" % (x[0], x[1]) for x in A ]) + ';\n')
fout.write('INSERT INTO Table2 VALUES ' + ','.join([ "(%d, '%s')" % (x[0], x[1]) for x in B ]) + ';\n')
fout.write('INSERT INTO Table3 VALUES ' + ','.join([ "(%d, '%s')" % (x[0], x[1]) for x in C ]) + ';\n')
fout.write('SELECT COUNT(*) FROM Table1, Table2 WHERE Table1.Key = Table2.Key;')
fout.write('SELECT COUNT(*) FROM Table2, Table3 WHERE Table2.Key = Table3.Key;')
fout.write('SELECT COUNT(*) FROM Table1, Table3 WHERE Table1.Key = Table3.Key;')
fout.write('SELECT COUNT(*) FROM Table1, Table2, Table3 WHERE Table1.Key = Table2.Key AND Table2.Key = Table3.Key;')

Ac, Bc, Cc = [0] * 100, [0] * 100, [0] * 100
for i in A:
    Ac[int(i[1])] += 1

for i in B:
    Bc[int(i[1])] += 1

for i in C:
    Cc[int(i[1])] += 1

Cab, Cac, Cbc, Cabc = 0, 0, 0, 0
for a, b in zip(Ac, Bc):
    Cab += a * b
for a, b in zip(Ac, Cc):
    Cac += a * b
for a, b in zip(Bc, Cc):
    Cbc += a * b
for a, b, c in zip(Ac, Bc, Cc):
    Cabc += a * b * c

fans.write('COUNT(*)\n%d\n' % Cab)
fans.write('COUNT(*)\n%d\n' % Cbc)
fans.write('COUNT(*)\n%d\n' % Cac)
fans.write('COUNT(*)\n%d\n' % Cabc)

fout.close()
fans.close()
