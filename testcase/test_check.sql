CREATE DATABASE db_primary;
SET OUTPUT = 'test_check.out';
USE db_primary;
CREATE TABLE Persons (
    PersonID int,
    Name varchar(20),
	Gender varchar(1) DEFAULT 'M',
	CHECK (PersonID > 10),
	CHECK (Gender IN ('F', 'M'))
);

SHOW TABLE Persons;

INSERT INTO Persons (PersonID, Name) VALUES 
	(10, 'Person10'),
	(11, 'Person11'),
	(12, 'Person12'),
	(2,  'Person2');

INSERT INTO Persons VALUES 
	(21, 'Person21', 'F'),
	(22, 'Person22', 'G'),
	(23, 'Person23', 'M'),
	(24, 'Person24', NULL);

SELECT * FROM Persons;

UPDATE Persons SET PersonID = PersonID / 2;

SELECT * FROM Persons;


EXIT;
