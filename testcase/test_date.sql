CREATE DATABASE db_date;
USE db_date;
SET OUTPUT = 'test_date.out';
CREATE TABLE Persons (
    PersonID int,
	Birthday date,
	Name varchar(20)
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(10, '1999-12-05', 'hello'),
	(11, '1999-03-12', 'world'),
	(11, '2009-04-09', '2001-10-10');

SELECT * FROM Persons;

EXIT;
