CREATE DATABASE db_unique;
SET OUTPUT = 'test_null.out';
USE db_unique;
CREATE TABLE Persons (
    PersonID int,
    Name varchar(20)
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(4, 'Person_4'),
	(5, 'Person_5');

INSERT INTO Persons (PersonID) VALUES 
	(6),
	(7);

INSERT INTO Persons (Name) VALUES 
	('Person_8'),
	('Person_9');

SELECT * FROM Persons;

SELECT * FROM Persons WHERE PersonID IS NULL;
SELECT * FROM Persons WHERE PersonID IS NOT NULL;
SELECT * FROM Persons WHERE PersonID > 0;
SELECT * FROM Persons WHERE Name = '';
SELECT * FROM Persons WHERE Name IS NULL;
SELECT * FROM Persons WHERE Name IS NOT NULL;

CREATE TABLE Persons2 (
    PersonID int NOT NULL,
    Name varchar(20)
);

INSERT INTO Persons2 VALUES 
	(4, 'Person_4'),
	(5, 'Person_5');

INSERT INTO Persons2 (PersonID) VALUES 
	(6),
	(7);

INSERT INTO Persons2 (Name) VALUES 
	('Person_8'),
	('Person_9');

SELECT * FROM Persons2;

EXIT;
