CREATE DATABASE db_unique;
SET OUTPUT = 'test_unique.out';
USE db_unique;
CREATE TABLE Persons (
    PersonID int UNIQUE,
    Name varchar(20) PRIMARY KEY,
	Other varchar(10)
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(4, 'Person_4', 'hello'),
	(5, 'Person_5', 'hello');

INSERT INTO Persons VALUES 
	(4, 'Person_4a', 'hello');

SELECT * FROM Persons;

INSERT INTO Persons VALUES 
	(6, 'Person_4a', 'hello');

INSERT INTO Persons VALUES 
	(7, 'Person_7', 'helloA');

SELECT * FROM Persons;

UPDATE Persons SET PersonID = 4 WHERE Name = 'Person_4a';

SELECT * FROM Persons;

EXIT;
