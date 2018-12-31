CREATE DATABASE db_default;
USE db_default;
SET OUTPUT = 'test_default.out';
CREATE TABLE Persons (
    PersonID int,
	Birthday date DEFAULT '1999-03-12',
	Name varchar(20) DEFAULT 'hello world'
);

SHOW TABLE Persons;

INSERT INTO Persons VALUES 
	(10, '1999-12-05', 'hello'),
	(11, '1999-03-12', 'world'),
	(11, NULL, 'world'),
	(11, '2018-09-10', NULL),
	(11, '2009-04-09', '2001-10-10');

INSERT INTO Persons (PersonID) VALUES 
	(100), (200);

SELECT * FROM Persons;

EXIT;
