CREATE DATABASE db;
USE db;
CREATE TABLE Persons (
    PersonID int,
    LastName varchar(20),
    FirstName varchar(20),
    Address varchar(20),
    City varchar(10)
);
CREATE INDEX Persons(PersonID);
CREATE INDEX Persons(FirstName);
INSERT INTO Persons VALUES 
	(23, 'Yi', 'Wang', 'Tsinghua Univ.', 'Beijing'),
	(-238, 'Zhong', 'Lei', 'Beijing Univ.', 'Neijing'),
	(1+999, 'Wasserstein', 'Zhang', 'Hunan Univ.', 'Hunan');

SELECT PersonID FROM Persons;
UPDATE Persons SET LastName = 'Hilbert' WHERE PersonID = -238;
SELECT PersonID FROM Persons;
INSERT INTO Persons VALUES 
	(100001, 'Zarisk', 'C', 'Unknown', 'US'),
	(100002, 'Wasserstein', 'D', 'Unknwon.', 'EU');
DELETE FROM Persons WHERE PersonID < 0;
SELECT PersonID FROM Persons;
DELETE FROM Persons;
SELECT PersonID FROM Persons;
INSERT INTO Persons (LastName, PersonID) VALUES 
	('Zarisk', 10),
	('Wasserstein', 20);
SELECT PersonID FROM Persons;
EXIT;
