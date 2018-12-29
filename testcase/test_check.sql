CREATE DATABASE db_primary;
USE db_primary;
CREATE TABLE Persons (
    PersonID int,
    Name varchar(20),
	CHECK (PersonID > 10)
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(10, 'Person10'),
	(11, 'Person11'),
	(12, 'Person12'),
	(2,  'Person2');

SELECT * FROM Persons;

EXIT;
