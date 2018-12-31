CREATE DATABASE db_link;
SET OUTPUT = 'test_link.out';
USE db_link;
CREATE TABLE Persons (
    PersonID int PRIMARY KEY,
    Name varchar(20)
);

CREATE TABLE Conn (
    ConnID int PRIMARY KEY,
    Name varchar(20)
);

CREATE TABLE Conn2 (
    ConnID int,
    Name varchar(20)
);
CREATE INDEX Persons(Name);

SHOW TABLE Persons;
SHOW TABLE Conn;
SHOW TABLE Conn2;

INSERT INTO Conn VALUES 
	(9, 'Conn_9'),
	(19, 'Conn_19'),
	(5, 'Conn_5'),
	(8, 'Conn_8');

INSERT INTO Conn2 VALUES 
	(9, 'Conn2_9'),
	(19, 'Conn2_19'),
	(5, 'Conn2_5'),
	(8, 'Conn2_8');

INSERT INTO Persons VALUES 
	(4, 'Person_4'),
	(5, 'Person_5'),
	(6, 'Person_6'),
	(8, 'Person_8');

SELECT * FROM Persons, Conn, Conn2 WHERE Conn2.ConnID = Conn.ConnID AND Conn.ConnID = Persons.PersonID;
SELECT * FROM Persons, Conn WHERE Persons.PersonID = Conn.ConnID;
DELETE FROM Conn WHERE ConnID = 8;
SELECT * FROM Persons, Conn WHERE Persons.PersonID = Conn.ConnID;
UPDATE Conn SET ConnID = 8 WHERE ConnID = 19;
SELECT * FROM Persons, Conn WHERE Persons.PersonID = Conn.ConnID;

EXIT;
