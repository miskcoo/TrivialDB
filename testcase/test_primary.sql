CREATE DATABASE db_primary;
SET OUTPUT = 'test_primary.out';
USE db_primary;
CREATE TABLE Persons (
    PersonID int PRIMARY KEY,
    Name varchar(20) PRIMARY KEY,
	Other varchar(10)
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(4, 'Person_4', 'hello'),
	(6, 'Person_4', 'hello'),
	(8, 'Person_4', 'hello'),
	(5, 'Person_5', 'hello');

INSERT INTO Persons VALUES 
	(4, 'Person_4a', 'hello');

SELECT * FROM Persons;

INSERT INTO Persons VALUES 
	(4, 'Person_4a', 'hello');

INSERT INTO Persons VALUES 
	(5, 'Person_4a', 'helloA');

SELECT * FROM Persons;
UPDATE Persons SET PersonID = 4 WHERE Other = 'helloA';
SELECT * FROM Persons;
UPDATE Persons SET PersonID = 4 WHERE Name = 'Person_4a';
SELECT * FROM Persons;
UPDATE Persons SET PersonID = 6 WHERE Name = 'Person_4a';
SELECT * FROM Persons;

EXIT;
