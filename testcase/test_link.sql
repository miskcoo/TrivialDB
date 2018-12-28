CREATE DATABASE db_link;
USE db_link;
CREATE TABLE Persons (
    PersonID int,
    Name varchar(20)
);

CREATE TABLE Conn (
    ConnID int,
    Name varchar(20)
);
CREATE INDEX Persons(PersonID);
CREATE INDEX Persons(Name);
CREATE INDEX Conn(ConnID);

INSERT INTO Conn VALUES 
	(9, 'Conn_9'),
	(19, 'Conn_19'),
	(5, 'Conn_5'),
	(8, 'Conn_8');

INSERT INTO Persons VALUES 
	(4, 'Person_4'),
	(5, 'Person_5'),
	(6, 'Person_6'),
	(8, 'Person_8');

SELECT * FROM Persons, Conn WHERE Persons.PersonID = Conn.ConnID;

EXIT;
