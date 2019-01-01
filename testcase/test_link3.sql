CREATE DATABASE db_link3;
SET OUTPUT = 'test_link3.out';
USE db_link3;
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
CREATE INDEX Conn(Name);

INSERT INTO Conn VALUES 
	(1, 'A'),
	(2, 'A'),
	(3, 'A'),
	(4, 'B');

INSERT INTO Conn2 VALUES 
	(1, 'B'),
	(2, 'B'),
	(3, 'A'),
	(4, 'A');

INSERT INTO Persons VALUES 
	(1, 'A'),
	(2, 'B'),
	(3, 'B'),
	(4, 'B');

SELECT * FROM Persons, Conn, Conn2 WHERE Persons.Name = Conn.Name AND Conn.Name = Conn2.Name;
SELECT * FROM Persons, Conn, Conn2 WHERE Persons.Name = Conn.Name AND Conn.ConnID = Conn2.ConnID;
SELECT * FROM Persons, Conn, Conn2 WHERE Persons.Name = Conn2.Name AND Conn.ConnID = Conn2.ConnID;
EXIT;
