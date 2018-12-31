CREATE DATABASE db_foreign;
USE db_foreign;
SET OUTPUT = 'test_foreign.out';

CREATE TABLE Persons (
    PersonID int PRIMARY KEY,
    Name varchar(20)
);

CREATE TABLE Conn (
    ConnID int,
    Name varchar(20),
	FOREIGN KEY (ConnID) REFERENCES Persons(PersonID)
);

SHOW TABLE Persons;
SHOW TABLE Conn;

INSERT INTO Persons VALUES 
	(4, 'Person_4'),
	(5, 'Person_5'),
	(6, 'Person_6'),
	(8, 'Person_8');

INSERT INTO Conn VALUES 
	(9, 'Conn_9'),
	(4, 'Conn_4'),
	(19, 'Conn_19'),
	(5, 'Conn_5'),
	(8, 'Conn_8');

SELECT * FROM Conn;
DELETE FROM Persons WHERE PersonID = 5;
SELECT * FROM Conn;
UPDATE Conn SET ConnID = 14 WHERE ConnID = 5;
SELECT * FROM Conn;
UPDATE Conn SET ConnID = 4 WHERE ConnID = 5;
SELECT * FROM Conn;

EXIT;
